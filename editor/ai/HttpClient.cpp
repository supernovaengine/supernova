// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "HttpClient.h"

#include <cctype>
#include <fstream>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string_view>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #include <winhttp.h>
#else
    #include <curl/curl.h>
#endif

namespace fs = std::filesystem;

namespace doriax::editor::ai {

namespace {

// Receives response body bytes as they arrive; return false to signal a write
// failure. Both send() (string sink) and downloadToFile() (file sink) reuse the
// single backend performRequest() below through this.
using BodySink = std::function<bool(const char*, size_t)>;

std::string requestTimeoutError(long timeoutSeconds) {
    return "Request timed out (configured timeout: " + std::to_string(timeoutSeconds) +
           " seconds). Raise \"Request Timeout (s)\" in AI settings for slow local models.";
}

#ifdef _WIN32

// ---------------------------------------------------------------------------
// WinHTTP backend (native on Windows; no libcurl or curl CLI dependency)
// ---------------------------------------------------------------------------

std::wstring toWide(const std::string& utf8) {
    if (utf8.empty()) return {};
    const int len = MultiByteToWideChar(CP_UTF8, 0, utf8.data(),
                                        static_cast<int>(utf8.size()), nullptr, 0);
    std::wstring wide(static_cast<size_t>(len), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()),
                        wide.data(), len);
    return wide;
}

std::string toUtf8(const std::wstring& wide) {
    if (wide.empty()) return {};
    const int len = WideCharToMultiByte(CP_UTF8, 0, wide.data(),
                                        static_cast<int>(wide.size()), nullptr, 0,
                                        nullptr, nullptr);
    std::string utf8(static_cast<size_t>(len), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()),
                        utf8.data(), len, nullptr, nullptr);
    return utf8;
}

std::string winHttpError(const char* context, long timeoutSeconds = 0) {
    const DWORD error = GetLastError();
    if (error == ERROR_WINHTTP_TIMEOUT && timeoutSeconds > 0) {
        return requestTimeoutError(timeoutSeconds);
    }
    return std::string(context) + " (WinHTTP error " + std::to_string(error) + ")";
}

// RAII wrapper so every error path closes its handles automatically.
struct Handle {
    HINTERNET h = nullptr;
    Handle() = default;
    explicit Handle(HINTERNET handle) : h(handle) {}
    ~Handle() { if (h) WinHttpCloseHandle(h); }
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
    operator HINTERNET() const { return h; }
    explicit operator bool() const { return h != nullptr; }
};

void appendResponseHeader(HINTERNET request, const wchar_t* queryName,
                          const char* storedName, HttpResponse& response) {
    DWORD size = 0;
    WinHttpQueryHeaders(request, WINHTTP_QUERY_CUSTOM, queryName, nullptr, &size,
                        WINHTTP_NO_HEADER_INDEX);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || size < sizeof(wchar_t)) {
        return;
    }

    std::wstring value(size / sizeof(wchar_t), L'\0');
    if (!WinHttpQueryHeaders(request, WINHTTP_QUERY_CUSTOM, queryName,
                             value.data(), &size, WINHTTP_NO_HEADER_INDEX)) {
        return;
    }
    value.resize(size / sizeof(wchar_t));
    while (!value.empty() && value.back() == L'\0') {
        value.pop_back();
    }
    response.headers.emplace_back(storedName, toUtf8(value));
}

HttpResponse performRequest(const HttpRequest& request,
                            const BodySink& sink,
                            const std::atomic<bool>* cancel) {
    HttpResponse response;

    Handle session(WinHttpOpen(L"DoriaxEditorAI/1.0",
                               WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                               WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
    if (!session) {
        response.error = winHttpError("WinHttpOpen failed");
        return response;
    }

    DWORD timeoutMs = static_cast<DWORD>(request.timeoutSeconds) * 1000;
    WinHttpSetTimeouts(session, timeoutMs, timeoutMs, timeoutMs, timeoutMs);

    // AI endpoints require modern TLS; pre-22H2 Windows still defaults WinHTTP
    // to TLS 1.0/1.1, so opt in explicitly.
    DWORD protocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
#ifdef WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3
    protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3;
#endif
    WinHttpSetOption(session, WINHTTP_OPTION_SECURE_PROTOCOLS, &protocols, sizeof(protocols));

    const std::wstring url = toWide(request.url);
    URL_COMPONENTS parts{};
    parts.dwStructSize = sizeof(parts);
    parts.dwHostNameLength = static_cast<DWORD>(-1);
    parts.dwUrlPathLength = static_cast<DWORD>(-1);
    parts.dwExtraInfoLength = static_cast<DWORD>(-1);
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts)) {
        response.error = winHttpError("WinHttpCrackUrl failed");
        return response;
    }
    const std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
    std::wstring path(parts.lpszUrlPath, parts.dwUrlPathLength + parts.dwExtraInfoLength);
    if (path.empty()) path = L"/";
    const bool secure = parts.nScheme == INTERNET_SCHEME_HTTPS;

    Handle connect(WinHttpConnect(session, host.c_str(), parts.nPort, 0));
    if (!connect) {
        response.error = winHttpError("WinHttpConnect failed", request.timeoutSeconds);
        return response;
    }

    const std::wstring method = toWide(request.method);
    Handle req(WinHttpOpenRequest(connect, method.c_str(), path.c_str(), nullptr,
                                  WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                                  secure ? WINHTTP_FLAG_SECURE : 0));
    if (!req) {
        response.error = winHttpError("WinHttpOpenRequest failed");
        return response;
    }

    // WinHttpSetTimeouts does not change WinHTTP's separate 90-second wait for
    // response headers. Local inference servers may not send those headers until
    // generation finishes, so apply the configured request timeout here too.
    if (!WinHttpSetOption(req, WINHTTP_OPTION_RECEIVE_RESPONSE_TIMEOUT,
                          &timeoutMs, sizeof(timeoutMs))) {
        response.error = winHttpError("Failed to set the response-header timeout");
        return response;
    }

    std::wstring headerBlock;
    for (const auto& header : request.headers) {
        if (!headerBlock.empty()) headerBlock += L"\r\n";
        headerBlock += toWide(header);
    }

    if (cancel && cancel->load()) {
        response.error = "Request cancelled";
        return response;
    }

    if (!WinHttpSendRequest(req,
            headerBlock.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : headerBlock.c_str(),
            headerBlock.empty() ? 0 : static_cast<DWORD>(-1),
            request.body.empty() ? WINHTTP_NO_REQUEST_DATA
                                 : const_cast<char*>(request.body.data()),
            static_cast<DWORD>(request.body.size()),
            static_cast<DWORD>(request.body.size()), 0)) {
        response.error = winHttpError("WinHttpSendRequest failed", request.timeoutSeconds);
        return response;
    }

    if (!WinHttpReceiveResponse(req, nullptr)) {
        response.error = winHttpError("WinHttpReceiveResponse failed", request.timeoutSeconds);
        return response;
    }

    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    WinHttpQueryHeaders(req, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                        WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusSize,
                        WINHTTP_NO_HEADER_INDEX);
    response.status = statusCode;
    appendResponseHeader(req, L"Retry-After", "retry-after", response);
    appendResponseHeader(req, L"x-ratelimit-reset-tokens",
                         "x-ratelimit-reset-tokens", response);
    appendResponseHeader(req, L"x-ratelimit-reset-project-tokens",
                         "x-ratelimit-reset-project-tokens", response);
    appendResponseHeader(req, L"x-ratelimit-reset-requests",
                         "x-ratelimit-reset-requests", response);

    while (true) {
        if (cancel && cancel->load()) {
            response.error = "Request cancelled";
            break;
        }
        DWORD available = 0;
        if (!WinHttpQueryDataAvailable(req, &available)) {
            response.error = winHttpError("WinHttpQueryDataAvailable failed", request.timeoutSeconds);
            break;
        }
        if (available == 0) break;

        std::string buffer(available, '\0');
        DWORD read = 0;
        if (!WinHttpReadData(req, buffer.data(), available, &read)) {
            response.error = winHttpError("WinHttpReadData failed", request.timeoutSeconds);
            break;
        }
        if (read == 0) break;
        if (!sink(buffer.data(), read)) {
            response.error = "Failed to write response body";
            break;
        }
    }

    return response;
}

#else

// ---------------------------------------------------------------------------
// libcurl backend (Linux, macOS, and other non-Windows platforms)
// ---------------------------------------------------------------------------

size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    const auto* sink = static_cast<const BodySink*>(userdata);
    const size_t total = size * nmemb;
    return (*sink)(ptr, total) ? total : 0;
}

size_t headerCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* response = static_cast<HttpResponse*>(userdata);
    const size_t total = size * nmemb;
    std::string_view line(ptr, total);
    const size_t colon = line.find(':');
    if (colon == std::string_view::npos) {
        return total;
    }

    size_t valueStart = colon + 1;
    while (valueStart < line.size() &&
           std::isspace(static_cast<unsigned char>(line[valueStart]))) {
        ++valueStart;
    }
    size_t valueEnd = line.size();
    while (valueEnd > valueStart &&
           std::isspace(static_cast<unsigned char>(line[valueEnd - 1]))) {
        --valueEnd;
    }
    response->headers.emplace_back(
        std::string(line.substr(0, colon)),
        std::string(line.substr(valueStart, valueEnd - valueStart)));
    return total;
}

int progressCallback(void* clientp, curl_off_t, curl_off_t, curl_off_t, curl_off_t) {
    const auto* cancel = static_cast<const std::atomic<bool>*>(clientp);
    return (cancel && cancel->load()) ? 1 : 0;
}

curl_slist* buildHeaders(const std::vector<std::string>& headers) {
    curl_slist* list = nullptr;
    for (const auto& header : headers) {
        list = curl_slist_append(list, header.c_str());
    }
    return list;
}

HttpResponse performRequest(const HttpRequest& request,
                            const BodySink& sink,
                            const std::atomic<bool>* cancel) {
    HttpResponse response;
    CURL* curl = curl_easy_init();
    if (!curl) {
        response.error = "Failed to initialize libcurl";
        return response;
    }

    curl_slist* headers = buildHeaders(request.headers);

    curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, request.timeoutSeconds);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "DoriaxEditorAI/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sink);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progressCallback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, cancel);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    if (headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }
    if (request.method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(request.body.size()));
    }

    const CURLcode result = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status);
    if (result != CURLE_OK) {
        response.error = result == CURLE_OPERATION_TIMEDOUT
            ? requestTimeoutError(request.timeoutSeconds)
            : curl_easy_strerror(result);
    }

    if (headers) {
        curl_slist_free_all(headers);
    }
    curl_easy_cleanup(curl);
    return response;
}

#endif

} // namespace

HttpResponse HttpClient::send(const HttpRequest& request, const std::atomic<bool>* cancel) const {
    if (cancel && cancel->load()) {
        return {0, {}, "Request cancelled"};
    }

    std::string body;
    HttpResponse response = performRequest(request,
        [&body](const char* data, size_t size) {
            body.append(data, size);
            return true;
        },
        cancel);
    response.body = std::move(body);

    if (cancel && cancel->load()) {
        response.error = "Request cancelled";
    }
    return response;
}

HttpResponse HttpClient::get(const std::string& url,
                             const std::vector<std::string>& headers,
                             const std::atomic<bool>* cancel) const {
    HttpRequest request;
    request.method = "GET";
    request.url = url;
    request.headers = headers;
    return send(request, cancel);
}

HttpResponse HttpClient::downloadToFile(const std::string& url,
                                        const fs::path& destination,
                                        const std::vector<std::string>& headers,
                                        const std::atomic<bool>* cancel) const {
    if (cancel && cancel->load()) {
        return {0, {}, "Request cancelled"};
    }
    if (destination.has_parent_path()) {
        std::error_code ec;
        fs::create_directories(destination.parent_path(), ec);
        if (ec) {
            return {0, {}, "Failed to create destination directory: " + ec.message()};
        }
    }

    std::ofstream file(destination, std::ios::binary | std::ios::trunc);
    if (!file) {
        return {0, {}, "Failed to open destination file"};
    }

    HttpRequest request;
    request.method = "GET";
    request.url = url;
    request.headers = headers;
    request.timeoutSeconds = 300;

    HttpResponse response = performRequest(request,
        [&file](const char* data, size_t size) {
            file.write(data, static_cast<std::streamsize>(size));
            return static_cast<bool>(file);
        },
        cancel);
    file.close();

    if (cancel && cancel->load()) {
        response.error = "Request cancelled";
    }
    if (!response.error.empty() || response.status < 200 || response.status >= 300) {
        std::error_code ec;
        fs::remove(destination, ec);
    }
    return response;
}

std::string HttpClient::urlEncode(const std::string& value) {
    std::ostringstream encoded;
    encoded << std::hex << std::uppercase;
    for (unsigned char c : value) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << static_cast<char>(c);
        } else {
            encoded << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(c);
        }
    }
    return encoded.str();
}

} // namespace doriax::editor::ai
