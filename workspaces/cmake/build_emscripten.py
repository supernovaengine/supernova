#!/usr/bin/env python

# /*
# Based on Filip Stoklas code (http://forums.4fips.com/viewtopic.php?f=3&t=1201)
# */

import os
import stat
from shutil import rmtree
from subprocess import check_call

def resolve_path(rel_path):
    return os.path.abspath(os.path.join(os.path.dirname(__file__), rel_path))

def rmtree_silent(root):
    def remove_readonly_handler(fn, root, excinfo):
        if fn is os.rmdir:
            if os.path.isdir(root): # if exists
                os.chmod(root, stat.S_IWRITE) # make writable
                os.rmdir(root)
        elif fn is os.remove:
            if os.path.isfile(root): # if exists
                os.chmod(root, stat.S_IWRITE) # make writable
                os.remove(root)
    rmtree(root, onerror=remove_readonly_handler)

def makedirs_silent(root):
    try:
        os.makedirs(root)
    except OSError: # mute if exists
        pass

if __name__ == "__main__":

    if not "EMSCRIPTEN" in os.environ:
        print ("You need to include EMSCRIPTEN environment variable or edit 'emscripten =' line in this file")
    else:

        build_dir = resolve_path("./build")
        if not os.path.isdir(build_dir):
            rmtree_silent(build_dir)
        makedirs_silent(build_dir)
        os.chdir(build_dir)

        emscripten = os.path.expandvars("$EMSCRIPTEN")

        from sys import platform
        if platform == "linux" or platform == "linux2" or platform == "darwin":
            system_output = "Unix Makefiles"
            system_make = "make"
        elif platform == "win32":
            system_output = "MinGW Makefiles"
            system_make = "mingw32-make"

        check_call([
        "cmake",
        "-DCMAKE_TOOLCHAIN_FILE="+emscripten+"/cmake/Modules/Platform/Emscripten.cmake",
        "-DCMAKE_BUILD_TYPE=Debug",
        "-G", system_output,
        "../../../platform/emscripten"
        ])

        check_call([system_make])
