// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef DoriaxWin_h
#define DoriaxWin_h

// Entry point of the Win32 backend for exported games. The window is WindowWin,
// input translation is WinInputRouter and the doriax::System surface is
// SystemWin, so what is left here is the frame loop, the WGL context and the
// build settings.
class DoriaxWin {
public:

    static int init(int argc, char **argv);
};

#endif /* DoriaxWin_h */
