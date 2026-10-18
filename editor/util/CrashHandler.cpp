// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "CrashHandler.h"

#include "EditorVersion.h"

#include <atomic>
#include <csignal>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>

#if defined(_WIN32)
    #include <windows.h>
    #include <dbghelp.h>
#else
    #include <fcntl.h>
    #include <unistd.h>
    #if defined(__APPLE__)
        #include <mach-o/dyld.h>
    #endif
    #if defined(__GLIBC__) || defined(__APPLE__)
        #include <execinfo.h>
        #define DORIAX_HAS_EXECINFO 1
    #endif
#endif

using namespace doriax;

namespace {

    // Prepared at install time: a crashing process may not be able to allocate anymore,
    // so nothing below builds a string or takes a lock.
#if defined(_WIN32)
    // wide, the ANSI code page can't hold every path
    wchar_t logPath[4096] = {0};
#else
    char logPath[4096] = {0};
#endif
    char exePath[4096] = {0};
    bool installed = false;

    // Bounds re-entry: a fault while reporting, or an exception escaping the terminate
    // handler, must not recurse. The second entry still reports, a third dies where it is.
    std::atomic<int> fatalEntries{0};

#if defined(_WIN32)
    // Not on the stack, a stack overflow leaves the handler little of it
    wchar_t wideName[4096];
    char moduleName[4096];

    // UTF-8, as the log
    void moduleFileName(HMODULE module, char* out, int outSize) {
        const DWORD length = GetModuleFileNameW(module, wideName, (DWORD)(sizeof(wideName) / sizeof(wideName[0])));
        if (length == 0 || length >= sizeof(wideName) / sizeof(wideName[0]) ||
                WideCharToMultiByte(CP_UTF8, 0, wideName, -1, out, outSize, nullptr, nullptr) == 0) {
            out[0] = '\0';
        }
    }
#endif

    void captureExecutablePath() {
    #if defined(_WIN32)
        moduleFileName(nullptr, exePath, (int)sizeof(exePath));
    #elif defined(__APPLE__)
        uint32_t size = (uint32_t)sizeof(exePath);
        if (_NSGetExecutablePath(exePath, &size) != 0) exePath[0] = '\0';
    #else
        const ssize_t length = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
        exePath[length > 0 ? length : 0] = '\0';
    #endif
    }

#if defined(_WIN32)
    using FileHandle = HANDLE;
    const FileHandle kInvalidFile = INVALID_HANDLE_VALUE;

    FileHandle openReportFile() {
        if (logPath[0] == L'\0') return kInvalidFile;
        return CreateFileW(logPath, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    }

    void closeReportFile(FileHandle file) {
        if (file != kInvalidFile) CloseHandle(file);
    }

    // Not fwrite: the CRT stdio lock can be held by whatever was printing when it crashed.
    void writeRaw(FileHandle file, const char* text, size_t length) {
        DWORD written = 0;
        if (file != kInvalidFile) {
            WriteFile(file, text, (DWORD)length, &written, nullptr);
        }
        HANDLE errorHandle = GetStdHandle(STD_ERROR_HANDLE);
        if (errorHandle && errorHandle != INVALID_HANDLE_VALUE) {
            WriteFile(errorHandle, text, (DWORD)length, &written, nullptr);
        }
    }
#else
    using FileHandle = int;
    const FileHandle kInvalidFile = -1;

    FileHandle openReportFile() {
        if (logPath[0] == '\0') return kInvalidFile;
        return open(logPath, O_WRONLY | O_CREAT | O_APPEND, 0644);
    }

    void closeReportFile(FileHandle file) {
        if (file != kInvalidFile) close(file);
    }

    void writeRaw(FileHandle file, const char* text, size_t length) {
        ssize_t ignored = 0;
        if (file != kInvalidFile) ignored = write(file, text, length);
        ignored = write(2, text, length);
        (void)ignored;
    }
#endif

    void writeLine(FileHandle file, const char* text) {
        writeRaw(file, text, std::strlen(text));
        writeRaw(file, "\n", 1);
    }

    void writeFormatted(FileHandle file, const char* format, ...) {
        char buffer[1024];
        va_list args;
        va_start(args, format);
        int length = vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        if (length > 0) {
            if (length >= (int)sizeof(buffer)) length = (int)sizeof(buffer) - 1;
            writeRaw(file, buffer, (size_t)length);
        }
    }

#if defined(_WIN32)
    void writeFrame(FileHandle file, int index, DWORD64 address, HANDLE process, bool symbols, SYMBOL_INFO* symbol) {
        // A Release build has no .pdb to name anything, module+offset still locates it.
        moduleName[0] = '\0';
        unsigned long long moduleOffset = 0;
        HMODULE module = nullptr;
        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)(uintptr_t)address, &module) && module) {
            moduleFileName(module, moduleName, (int)sizeof(moduleName));
            moduleOffset = (unsigned long long)(address - (DWORD64)(uintptr_t)module);
        }

        DWORD64 displacement = 0;
        if (symbols && SymFromAddr(process, address, &displacement, symbol)) {
            writeFormatted(file, "  #%d %s(+0x%llx) %s+0x%llx\n", index,
                moduleName[0] ? moduleName : "?", moduleOffset,
                symbol->Name, (unsigned long long)displacement);
        } else {
            writeFormatted(file, "  #%d %s(+0x%llx)\n", index,
                moduleName[0] ? moduleName : "?", moduleOffset);
        }
    }
#endif

    // faultContext starts the walk where it faulted, a handler's own stack can stop at the dispatcher
    void writeBacktrace(FileHandle file, const void* faultContext = nullptr) {
#if defined(_WIN32)
        HANDLE process = GetCurrentProcess();
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
        const bool symbols = SymInitialize(process, nullptr, TRUE) != FALSE;

        char symbolBuffer[sizeof(SYMBOL_INFO) + 256] = {0};
        SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = 255;

        int count = 0;
    #if defined(_M_X64) || defined(__x86_64__)
        if (faultContext) {
            // a copy, StackWalk64 changes it, and static like the buffers above
            static CONTEXT context;
            context = *static_cast<const CONTEXT*>(faultContext);
            STACKFRAME64 frame = {};
            frame.AddrPC.Offset = context.Rip;
            frame.AddrPC.Mode = AddrModeFlat;
            frame.AddrFrame.Offset = context.Rbp;
            frame.AddrFrame.Mode = AddrModeFlat;
            frame.AddrStack.Offset = context.Rsp;
            frame.AddrStack.Mode = AddrModeFlat;
            while (count < 64 && StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, GetCurrentThread(), &frame, &context,
                    nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr) && frame.AddrPC.Offset != 0) {
                writeFrame(file, count++, frame.AddrPC.Offset, process, symbols, symbol);
            }
        }
    #endif
        if (count == 0) {
            void* frames[64];
            const USHORT captured = CaptureStackBackTrace(0, 64, frames, nullptr);
            for (USHORT i = 0; i < captured; i++) {
                writeFrame(file, count++, (DWORD64)(uintptr_t)frames[i], process, symbols, symbol);
            }
        }
        if (count == 0) writeLine(file, "  <no backtrace available>");

        if (symbols) SymCleanup(process);
#elif defined(DORIAX_HAS_EXECINFO)
        void* frames[64];
        int count = backtrace(frames, 64);
        if (count <= 0) {
            writeLine(file, "  <no backtrace available>");
            return;
        }
        // backtrace_symbols() allocates, this one writes straight to the descriptor.
        if (file != kInvalidFile) backtrace_symbols_fd(frames, count, file);
        backtrace_symbols_fd(frames, count, 2);
#else
        writeLine(file, "  <backtrace not supported on this platform>");
#endif
    }

    void report(const char* reason, const void* faultContext = nullptr) {
        FileHandle file = openReportFile();

        writeLine(file, "");
        writeLine(file, "========================================================");
        writeFormatted(file, "FATAL: %s\n", reason);
        writeFormatted(file, "Doriax editor %s\n", DORIAX_EDITOR_VERSION);
        if (exePath[0] != '\0') {
            writeFormatted(file, "binary: %s\n", exePath);
        }
        // An optimized build names few frames: resolve module+offset against this binary.
        writeLine(file, "backtrace:");
        writeBacktrace(file, faultContext);
        writeLine(file, "========================================================");
        writeLine(file, "");

        closeReportFile(file);
    }

    bool enterFatalReport() {
        return fatalEntries.fetch_add(1) < 2;
    }

    void handleSignal(int signalNumber) {
        // Restored first, so a fault while reporting dies instead of coming back here.
        std::signal(signalNumber, SIG_DFL);

        if (!enterFatalReport()) {
            std::raise(signalNumber);
            return;
        }

        const char* name = "signal";
        switch (signalNumber) {
            case SIGSEGV: name = "SIGSEGV (invalid memory access)"; break;
            case SIGABRT: name = "SIGABRT (abort)"; break;
            case SIGILL:  name = "SIGILL (illegal instruction)"; break;
            case SIGFPE:  name = "SIGFPE (arithmetic error)"; break;
#if !defined(_WIN32)
            case SIGBUS:  name = "SIGBUS (bus error)"; break;
#endif
            default: break;
        }

        report(name);

        // Back on the default handler, so the OS still writes its usual crash artifacts.
        std::raise(signalNumber);
    }

    void handleTerminate() {
        std::signal(SIGABRT, SIG_DFL);

        if (!enterFatalReport()) {
            std::abort();
        }

        char reason[512];
        snprintf(reason, sizeof(reason), "std::terminate with no active exception");

        // Rethrowing is what names the exception. A throw that finds no handler calls
        // terminate before unwinding, so the backtrace still reaches the throw site.
        if (std::exception_ptr active = std::current_exception()) {
            try {
                std::rethrow_exception(active);
            } catch (const std::exception& e) {
                snprintf(reason, sizeof(reason), "unhandled exception: %s", e.what());
            } catch (...) {
                snprintf(reason, sizeof(reason), "unhandled exception of unknown type");
            }
        }

        report(reason);

        std::abort();
    }

#if defined(_WIN32)
    LONG WINAPI handleWindowsException(EXCEPTION_POINTERS* info) {
        SetUnhandledExceptionFilter(nullptr);

        if (!enterFatalReport()) {
            return EXCEPTION_CONTINUE_SEARCH;
        }

        char reason[256];
        const EXCEPTION_RECORD* record = info ? info->ExceptionRecord : nullptr;
        const unsigned long code = record ? (unsigned long)record->ExceptionCode : 0;
        if (code == EXCEPTION_ACCESS_VIOLATION && record->NumberParameters >= 2) {
            const ULONG_PTR operation = record->ExceptionInformation[0];
            snprintf(reason, sizeof(reason), "access violation %s 0x%llx",
                operation == 1 ? "writing" : (operation == 8 ? "executing" : "reading"),
                (unsigned long long)record->ExceptionInformation[1]);
        } else if (code == EXCEPTION_STACK_OVERFLOW) {
            snprintf(reason, sizeof(reason), "stack overflow");
        } else {
            snprintf(reason, sizeof(reason), "unhandled exception, code 0x%08lx", code);
        }
        report(reason, info ? info->ContextRecord : nullptr);

        return EXCEPTION_CONTINUE_SEARCH;
    }
#endif

}

void editor::CrashHandler::install(const std::filesystem::path& logFile) {
#if defined(_WIN32)
    const std::wstring path = logFile.wstring();
#else
    const std::string path = logFile.string();
#endif
    if (path.size() < sizeof(logPath) / sizeof(logPath[0])) {
        std::memcpy(logPath, path.c_str(), (path.size() + 1) * sizeof(logPath[0]));
    }

    if (installed) {
        return;
    }
    installed = true;

    captureExecutablePath();

    std::set_terminate(handleTerminate);

    std::signal(SIGABRT, handleSignal);
#if defined(_WIN32)
    // faults reach the filter with their context, a CRT signal handler gets none
    SetUnhandledExceptionFilter(handleWindowsException);
#else
    std::signal(SIGSEGV, handleSignal);
    std::signal(SIGILL, handleSignal);
    std::signal(SIGFPE, handleSignal);
    std::signal(SIGBUS, handleSignal);
#endif
}
