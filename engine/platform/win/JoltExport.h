// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef JoltExport_h
#define JoltExport_h

// Forced into Jolt sources on MinGW, where Core.h never uses dllexport
#include "Jolt/Core/Core.h"

#undef JPH_EXPORT
#define JPH_EXPORT __declspec(dllexport)
#undef JPH_EXPORT_GCC_BUG_WORKAROUND
#define JPH_EXPORT_GCC_BUG_WORKAROUND [[gnu::dllexport]]

#endif /* JoltExport_h */
