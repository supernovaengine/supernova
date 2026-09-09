// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>

namespace doriax::editor {

    // Appends the uncaught exception or signal that kills the editor, plus a backtrace, to
    // the log file. Without it the window just disappears and the user has nothing to send.
    class CrashHandler {
    public:
        static void install(const std::filesystem::path& logFile);
    };

}
