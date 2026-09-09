// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ShellEnv.h"

#include <cstdlib>
#include <filesystem>
#include <vector>

#ifndef _WIN32
    #include <errno.h>
    #include <fcntl.h>
    #include <poll.h>
    #include <pwd.h>
    #include <signal.h>
    #include <spawn.h>
    #include <sys/wait.h>
    #include <time.h>
    #include <unistd.h>

    extern char **environ;
#endif

namespace fs = std::filesystem;

using namespace doriax;

namespace {
    #ifdef _WIN32
        constexpr char kPathSeparator = ';';
    #else
        constexpr char kPathSeparator = ':';
        constexpr int kProbeTimeoutMs = 5000;
        constexpr size_t kMaxProbeOutput = 64 * 1024;

        // The captured PATH is framed by markers: an interactive startup file is
        // free to print a banner or a greeting before it.
        const std::string kPathBegin = "__DORIAX_PATH_BEGIN__";
        const std::string kPathEnd = "__DORIAX_PATH_END__";
    #endif

    std::vector<std::string> splitPath(const std::string& value) {
        std::vector<std::string> entries;
        std::string current;
        for (char c : value) {
            if (c == kPathSeparator) {
                if (!current.empty()) entries.push_back(current);
                current.clear();
            } else {
                current += c;
            }
        }
        if (!current.empty()) entries.push_back(current);
        return entries;
    }

    std::string joinPath(const std::vector<std::string>& entries) {
        std::string value;
        for (const auto& entry : entries) {
            if (!value.empty()) value += kPathSeparator;
            value += entry;
        }
        return value;
    }

    std::string currentPath() {
        const char* value = std::getenv("PATH");
        return value ? std::string(value) : std::string();
    }

    std::string homeDir() {
    #ifdef _WIN32
        const char* home = std::getenv("USERPROFILE");
    #else
        const char* home = std::getenv("HOME");
    #endif
        return home ? std::string(home) : std::string();
    }

    // A trailing slash makes the same directory look like a second entry.
    std::string normalizeDir(const std::string& dir) {
        std::string out = dir;
        while (out.size() > 1 && (out.back() == '/' || out.back() == '\\')) {
            out.pop_back();
        }
        return out;
    }

    void appendUnique(std::vector<std::string>& entries, const std::string& dir) {
        const std::string normalized = normalizeDir(dir);
        if (normalized.empty()) return;
        for (const auto& existing : entries) {
            if (normalizeDir(existing) == normalized) return;
        }
        entries.push_back(normalized);
    }

    // Where the package managers and installers put their binaries. Appended
    // after everything the shell already provides, so they only fill gaps.
    std::vector<std::string> wellKnownToolDirs() {
        std::vector<std::string> dirs;
        const std::string home = homeDir();

    #if defined(__APPLE__)
        dirs.push_back("/opt/homebrew/bin");   // Homebrew on Apple silicon
        dirs.push_back("/opt/homebrew/sbin");
        dirs.push_back("/usr/local/bin");      // Homebrew on Intel, manual installs
        dirs.push_back("/usr/local/sbin");
        dirs.push_back("/opt/local/bin");      // MacPorts
        dirs.push_back("/opt/local/sbin");
        // CMake.app installs into no PATH directory unless the user runs its
        // "Install Command Line Tools" action.
        dirs.push_back("/Applications/CMake.app/Contents/bin");
        if (!home.empty()) {
            dirs.push_back(home + "/Applications/CMake.app/Contents/bin");
        }
    #elif !defined(_WIN32)
        dirs.push_back("/usr/local/bin");
        dirs.push_back("/usr/local/sbin");
        dirs.push_back("/home/linuxbrew/.linuxbrew/bin");
        if (!home.empty()) {
            dirs.push_back(home + "/.linuxbrew/bin");
        }
        dirs.push_back("/snap/bin");
        dirs.push_back("/var/lib/flatpak/exports/bin");
    #endif

    #ifndef _WIN32
        if (!home.empty()) {
            dirs.push_back(home + "/.local/bin");
        }
        // The launcher may hand over no PATH at all, so not even the system
        // directories can be assumed to be there.
        dirs.push_back("/usr/bin");
        dirs.push_back("/bin");
        dirs.push_back("/usr/sbin");
        dirs.push_back("/sbin");
    #endif

        return dirs;
    }

#ifndef _WIN32
    bool launchedFromTerminal() {
        return isatty(STDIN_FILENO) || isatty(STDOUT_FILENO) || isatty(STDERR_FILENO);
    }

    std::string userLoginShell() {
        const char* shell = std::getenv("SHELL");
        if (shell && *shell) return shell;

        if (const struct passwd* pw = getpwuid(getuid())) {
            if (pw->pw_shell && *pw->pw_shell) return pw->pw_shell;
        }
        return "/bin/sh";
    }

    // Runs one command through a shell and returns what it printed. stdin is
    // /dev/null and stderr is discarded, so an interactive startup file can
    // neither block on input nor pollute the output. Reading stops at
    // stopMarker: a shell is under no obligation to close stdout afterwards,
    // since a background job it started inherits the pipe and holds it open.
    // Sets aborted when the shell hung or flooded, which means asking it a
    // second time would only repeat the delay.
    std::string runShellCapture(const std::string& shell, const std::vector<std::string>& args, int timeoutMs, bool& aborted, const std::string& stopMarker = "") {
        aborted = false;

        int pipefd[2];
        if (pipe(pipefd) != 0) return "";

        posix_spawn_file_actions_t actions;
        posix_spawn_file_actions_init(&actions);
        posix_spawn_file_actions_addopen(&actions, STDIN_FILENO, "/dev/null", O_RDONLY, 0);
        posix_spawn_file_actions_addopen(&actions, STDERR_FILENO, "/dev/null", O_WRONLY, 0);
        posix_spawn_file_actions_addclose(&actions, pipefd[0]);
        posix_spawn_file_actions_adddup2(&actions, pipefd[1], STDOUT_FILENO);
        posix_spawn_file_actions_addclose(&actions, pipefd[1]);

        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(shell.c_str()));
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);

        // Own process group, so a startup file that hangs can be killed along
        // with anything it spawned instead of leaving strays behind.
        posix_spawnattr_t attr;
        posix_spawnattr_init(&attr);
        posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETPGROUP);
        posix_spawnattr_setpgroup(&attr, 0);

        pid_t pid = 0;
        const int spawnResult = posix_spawn(&pid, shell.c_str(), &actions, &attr, argv.data(), environ);
        posix_spawnattr_destroy(&attr);
        posix_spawn_file_actions_destroy(&actions);
        close(pipefd[1]);

        if (spawnResult != 0) {
            close(pipefd[0]);
            return "";
        }

        std::string output;
        const int pollStepMs = 50;
        int waitedMs = 0;
        size_t scanned = 0;
        bool timedOut = false;
        bool flooded = false;

        while (true) {
            struct pollfd pfd = { pipefd[0], POLLIN, 0 };
            const int ready = poll(&pfd, 1, pollStepMs);
            if (ready > 0) {
                char buffer[1024];
                const ssize_t bytes = read(pipefd[0], buffer, sizeof(buffer));
                if (bytes > 0) {
                    output.append(buffer, static_cast<size_t>(bytes));

                    if (!stopMarker.empty()) {
                        // Rescan from before the previous end, so a marker split
                        // across two reads is still found.
                        const size_t overlap = stopMarker.size() - 1;
                        const size_t from = scanned > overlap ? scanned - overlap : 0;
                        if (output.find(stopMarker, from) != std::string::npos) break;
                        scanned = output.size();
                    }

                    if (output.size() > kMaxProbeOutput) {
                        flooded = true;
                        break;
                    }
                    continue;
                }
                if (bytes == 0) break;                   // the shell closed stdout
                if (errno != EAGAIN && errno != EINTR) break;
            } else if (ready < 0 && errno != EINTR) {
                break;
            }

            waitedMs += pollStepMs;
            if (waitedMs >= timeoutMs) {
                timedOut = true;
                break;
            }
        }

        close(pipefd[0]);

        // The group signal reaches the shell's own children too. It is skipped
        // when POSIX_SPAWN_SETPGROUP did not take (the group would then be the
        // editor's), so the direct signal is what guarantees a reapable process.
        auto killShell = [pid]() {
            if (getpgid(pid) == pid) {
                kill(-pid, SIGKILL);
            }
            kill(pid, SIGKILL);
        };

        // Any exit from the loop can leave the shell alive, so a misbehaving one
        // is killed at once and the wait for the rest is bounded.
        if (timedOut || flooded) {
            killShell();
        }

        int status = 0;
        bool exited = false;
        for (int elapsedMs = 0; elapsedMs < 1000; elapsedMs += 20) {
            if (waitpid(pid, &status, WNOHANG) == pid) {
                exited = true;
                break;
            }
            struct timespec step = { 0, 20 * 1000 * 1000 };
            nanosleep(&step, nullptr);
        }

        if (!exited) {
            killShell();
            waitpid(pid, &status, 0);
        }

        aborted = timedOut || flooded;
        return output;
    }

    // Both markers must be there: a truncated capture is not a PATH.
    std::string extractMarked(const std::string& output) {
        const size_t from = output.find(kPathBegin);
        if (from == std::string::npos) return "";

        const size_t valueStart = from + kPathBegin.size();
        const size_t valueEnd = output.find(kPathEnd, valueStart);
        if (valueEnd == std::string::npos) return "";

        std::string value = output.substr(valueStart, valueEnd - valueStart);
        while (!value.empty() && (value.back() == '\n' || value.back() == '\r' || value.back() == ' ')) {
            value.pop_back();
        }
        return value;
    }

    // Asks the login shell for the PATH it exports, the only way to pick up one
    // assembled by ~/.zprofile, a version manager or a Homebrew shellenv line.
    std::string loginShellPath() {
        const std::string shell = userLoginShell();
        const std::string shellName = fs::path(shell).filename().string();

        // csh and tcsh reject clustered flags and read their startup files only
        // as a plain login shell, so they get the command alone.
        const bool cshFamily = (shellName == "csh" || shellName == "tcsh");

        // printenv reports the PATH as a child process receives it, which is
        // also right for fish, where $PATH is a list rather than a string.
        const std::vector<std::string> probes = {
            "printf '" + kPathBegin + "'; /usr/bin/printenv PATH; printf '" + kPathEnd + "'",
            "printf '" + kPathBegin + "%s" + kPathEnd + "' \"$PATH\""
        };

        for (const auto& probe : probes) {
            std::vector<std::string> args;
            if (!cshFamily) {
                args.push_back("-i");
                args.push_back("-l");
            }
            args.push_back("-c");
            args.push_back(probe);

            bool aborted = false;
            const std::string value = extractMarked(runShellCapture(shell, args, kProbeTimeoutMs, aborted, kPathEnd));
            if (!value.empty()) return value;
            if (aborted) break;
        }

        return "";
    }
#endif
}

void editor::ShellEnv::bootstrapPath() {
#ifndef _WIN32
    static bool bootstrapped = false;
    if (bootstrapped) return;
    bootstrapped = true;

    std::vector<std::string> merged;

    // The login shell's PATH comes first: its order is the precedence the user
    // configured. Started from a terminal the process already inherited that
    // PATH, so the probe is skipped and its cost avoided.
    if (!launchedFromTerminal()) {
        for (const auto& entry : splitPath(loginShellPath())) {
            appendUnique(merged, entry);
        }
    }

    for (const auto& entry : splitPath(currentPath())) {
        appendUnique(merged, entry);
    }

    for (const auto& dir : wellKnownToolDirs()) {
        std::error_code ec;
        if (fs::is_directory(dir, ec)) {
            appendUnique(merged, dir);
        }
    }

    const std::string path = joinPath(merged);
    if (!path.empty()) {
        setenv("PATH", path.c_str(), 1);
    }
#endif
}

#ifndef _WIN32
std::string editor::ShellEnv::runCapture(const std::string& command, int timeoutMs) {
    bool aborted = false;
    return runShellCapture("/bin/sh", { "-c", command }, timeoutMs, aborted);
}
#endif

std::string editor::ShellEnv::findExecutable(const std::string& command) {
    if (command.empty()) return "";

    std::error_code ec;

#ifdef _WIN32
    std::vector<std::string> candidates;
    candidates.push_back(command);
    const char* pathExt = std::getenv("PATHEXT");
    for (const auto& ext : splitPath(pathExt ? pathExt : ".COM;.EXE;.BAT;.CMD")) {
        candidates.push_back(command + ext);
    }

    for (const auto& dir : splitPath(currentPath())) {
        for (const auto& candidate : candidates) {
            const fs::path full = fs::path(dir) / candidate;
            if (fs::is_regular_file(full, ec)) return full.string();
        }
    }
#else
    for (const auto& dir : splitPath(currentPath())) {
        const fs::path full = fs::path(dir) / command;
        if (access(full.c_str(), X_OK) == 0 && !fs::is_directory(full, ec)) {
            return full.string();
        }
    }
#endif

    return "";
}
