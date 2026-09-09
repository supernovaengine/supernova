// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef DoriaxLinux_h
#define DoriaxLinux_h

// Entry point of the X11 backend for exported games. The window is WindowLinux,
// input translation is LinuxInputRouter and the doriax::System surface is
// SystemLinux, so what is left here is the frame loop and the GLX context.
class DoriaxLinux {
public:

    static int init(int argc, char **argv);
};

#endif /* DoriaxLinux_h */
