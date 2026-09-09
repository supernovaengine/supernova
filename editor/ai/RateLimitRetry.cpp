// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "RateLimitRetry.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <initializer_list>
#include <string_view>

namespace doriax::editor::ai {

namespace {

std::string lowercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

std::string trim(std::string value) {
    auto isSpace = [](unsigned char c) { return std::isspace(c) != 0; };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(),
                                            [&](unsigned char c) { return !isSpace(c); }));
    value.erase(std::find_if(value.rbegin(), value.rend(),
                             [&](unsigned char c) { return !isSpace(c); }).base(),
                value.end());
    return value;
}

std::string headerValue(const HttpResponse& response, const std::string& wantedName) {
    const std::string wanted = lowercase(wantedName);
    // Redirects can produce more than one header block; the final response is
    // appended last, so prefer its value.
    for (auto it = response.headers.rbegin(); it != response.headers.rend(); ++it) {
        if (lowercase(it->first) == wanted) {
            return trim(it->second);
        }
    }
    return {};
}

// Parses OpenAI reset durations such as "5.45s", "1m2s", and "250ms".
// A unitless value is accepted only when requested (Retry-After defines it as
// seconds). Text after the duration is harmless, which also supports provider
// messages such as "Please try again in 5.45s.".
std::chrono::milliseconds parseDuration(std::string_view text, bool unitlessSeconds) {
    double totalMilliseconds = 0.0;
    bool parsedAny = false;
    size_t pos = 0;

    while (pos < text.size()) {
        while (pos < text.size() &&
               std::isspace(static_cast<unsigned char>(text[pos]))) {
            ++pos;
        }

        const size_t numberStart = pos;
        bool sawDigit = false;
        bool sawDot = false;
        while (pos < text.size()) {
            const unsigned char c = static_cast<unsigned char>(text[pos]);
            if (std::isdigit(c)) {
                sawDigit = true;
                ++pos;
            } else if (text[pos] == '.' && !sawDot) {
                sawDot = true;
                ++pos;
            } else {
                break;
            }
        }
        if (!sawDigit) {
            break;
        }

        const std::string number(text.substr(numberStart, pos - numberStart));
        char* end = nullptr;
        const double amount = std::strtod(number.c_str(), &end);
        if (!end || *end != '\0' || !std::isfinite(amount) || amount < 0.0) {
            return std::chrono::milliseconds(0);
        }

        double multiplier = 0.0;
        if (pos + 1 < text.size() && text[pos] == 'm' && text[pos + 1] == 's') {
            multiplier = 1.0;
            pos += 2;
        } else if (pos < text.size() && text[pos] == 's') {
            multiplier = 1000.0;
            ++pos;
        } else if (pos < text.size() && text[pos] == 'm') {
            multiplier = 60.0 * 1000.0;
            ++pos;
        } else if (pos < text.size() && text[pos] == 'h') {
            multiplier = 60.0 * 60.0 * 1000.0;
            ++pos;
        } else if (!parsedAny && unitlessSeconds) {
            multiplier = 1000.0;
        } else {
            break;
        }

        totalMilliseconds += amount * multiplier;
        parsedAny = true;

        // A unitless Retry-After number is the complete value.
        if (multiplier == 1000.0 && unitlessSeconds &&
            (pos >= text.size() || text[pos] != 's')) {
            break;
        }
    }

    if (!parsedAny || totalMilliseconds <= 0.0) {
        return std::chrono::milliseconds(0);
    }
    return std::chrono::milliseconds(
        static_cast<int64_t>(std::ceil(totalMilliseconds)));
}

bool containsAny(const std::string& value,
                 std::initializer_list<const char*> needles) {
    for (const char* needle : needles) {
        if (value.find(needle) != std::string::npos) {
            return true;
        }
    }
    return false;
}

} // namespace

bool isRetryableRateLimit(long status, const ProviderResponse& error) {
    if (status != 429) {
        return false;
    }

    const std::string metadata = lowercase(
        error.errorCode + " " + error.errorType + " " + error.errorStatus);
    const std::string detail = lowercase(error.error);
    if (containsAny(metadata, {
            "insufficient_quota",
            "billing_hard_limit_reached",
            "billing_not_active",
            "payment_required",
            "usage_limit_reached"
        })) {
        return false;
    }

    // Some compatible providers omit structured codes. Keep this deny-list
    // deliberately billing-specific: generic "quota exceeded" language is
    // also used by Gemini for temporary per-minute RESOURCE_EXHAUSTED limits.
    if (containsAny(detail, {
            "insufficient quota",
            "check your plan and billing details",
            "check your billing details",
            "billing hard limit",
            "payment method is required"
        })) {
        return false;
    }

    // A 429 without structured metadata is normally a transient throttle.
    // Bounded retries in AiService prevent an unknown provider from looping.
    return true;
}

std::chrono::milliseconds rateLimitRetryDelay(
    const HttpResponse& response, const std::string& detail,
    const std::string& structuredRetryDelay) {
    std::chrono::milliseconds retryDelay(0);

    const std::string retryAfter = headerValue(response, "retry-after");
    if (!retryAfter.empty()) {
        retryDelay = std::max(
            retryDelay, parseDuration(retryAfter, true));
    }

    if (!structuredRetryDelay.empty()) {
        retryDelay = std::max(
            retryDelay, parseDuration(structuredRetryDelay, false));
    }

    const std::string lowerDetail = lowercase(detail);
    constexpr std::string_view marker = "try again in ";
    const size_t markerPos = lowerDetail.find(marker);
    if (markerPos != std::string::npos) {
        retryDelay = std::max(
            retryDelay,
            parseDuration(
                std::string_view(lowerDetail).substr(
                    markerPos + marker.size()),
                false));
    }

    if (retryDelay.count() > 0) {
        return retryDelay;
    }

    // With no precise instruction, conservatively wait for the slowest
    // applicable request/token bucket reset.
    for (const char* name : {
             "x-ratelimit-reset-tokens",
             "x-ratelimit-reset-project-tokens",
             "x-ratelimit-reset-requests"
         }) {
        const std::string value = headerValue(response, name);
        if (!value.empty()) {
            retryDelay = std::max(
                retryDelay, parseDuration(value, false));
        }
    }
    return retryDelay;
}

} // namespace doriax::editor::ai
