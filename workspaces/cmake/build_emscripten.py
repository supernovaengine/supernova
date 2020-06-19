#!/usr/bin/env python

# /*
# Based on Filip Stoklas code (http://forums.4fips.com/viewtopic.php?f=3&t=1201)
# (c) 2020 Eduardo Doria.
# */

import os
import sys
import subprocess
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

    emscripten = ""
    if "EMSCRIPTEN_ROOT" in os.environ:
        emscripten = os.path.expandvars("$EMSCRIPTEN_ROOT")
    elif "EMSCRIPTEN" in os.environ:
        emscripten = os.path.expandvars("$EMSCRIPTEN")
    else:
        print ("Not found EMSCRIPTEN_ROOT or EMSCRIPTEN environment variable")
        sys.exit(os.EX_CONFIG)

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

    subprocess.run([
    "cmake",
    "-DCMAKE_TOOLCHAIN_FILE="+emscripten+"/cmake/Modules/Platform/Emscripten.cmake",
    "-DCMAKE_BUILD_TYPE=Debug",
    "-G", system_output,
    "../../../platform/emscripten"
    ]).check_returncode()

    subprocess.run([system_make]).check_returncode()
