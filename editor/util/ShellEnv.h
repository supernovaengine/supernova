// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <string>

namespace doriax::editor {

    // PATH repair for an editor started by a desktop launcher (Finder,
    // Launchpad, a .desktop file). Such a process inherits the session daemon's
    // environment instead of the user's shell: on macOS that PATH is usually
    // empty, so the /bin/sh CommandRunner spawns falls back to "/usr/bin:/bin"
    // and finds neither a Homebrew nor a manual CMake install. Reading ~/.zshrc
    // would not help either, that shell being neither interactive nor a login
    // shell.
    class ShellEnv {
    public:
        // Merges the login shell's PATH and the usual tool directories into this
        // process's PATH. Does nothing on Windows, where the launcher already
        // passes the system and user PATH along, or on any call after the first.
        static void bootstrapPath();

        // Absolute path of a command found on PATH, or "" when it is not on it.
        static std::string findExecutable(const std::string& command);

#ifndef _WIN32
        // Runs a command through /bin/sh and returns its output, killing it
        // after timeoutMs. popen() has no timeout, and a GUI application asked
        // for --version never answers. Windows uses runCaptureNoWindow.
        static std::string runCapture(const std::string& command, int timeoutMs);
#endif
    };

}
