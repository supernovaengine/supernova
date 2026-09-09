// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Conector.h"

#include "editor/Out.h"
#include "Project.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <system_error>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

using namespace doriax;

editor::Conector::Conector(){
    libHandle = nullptr;
}

editor::Conector::~Conector(){
    disconnect();
}

bool editor::Conector::fileExists(const fs::path& path) {
    return std::filesystem::exists(path);
}

void* editor::Conector::loadSharedLibrary(const std::string& libPath) {
    #ifdef _WIN32
        HMODULE libHandle = LoadLibrary(libPath.c_str());
        if (!libHandle) {
            Out::error("Failed to load library: %s (Error code: %lu)", libPath.c_str(), GetLastError());
            return nullptr;
        }
        return libHandle;
    #else
        void* libHandle = dlopen(libPath.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!libHandle) {
            Out::error("Failed to load library: %s (Error: %s)", libPath.c_str(), dlerror());
            return nullptr;
        }
        return libHandle;
    #endif
}

void editor::Conector::unloadSharedLibrary(void* libHandle) {
    #ifdef _WIN32
        if (libHandle) {
            FreeLibrary(static_cast<HMODULE>(libHandle));
            std::cout << "Library unloaded successfully.\n";
        }
    #else
        if (libHandle) {
            dlclose(libHandle);
            std::cout << "Library unloaded successfully.\n";
        }
    #endif
}

bool editor::Conector::connect(const fs::path& buildPath, std::string libName){
    // Disconnect any existing connection first
    disconnect();

    std::string libPath = libName;
    #ifdef _WIN32
        libPath = libPath + ".dll";
    #elif defined(__APPLE__)
        libPath = libPath + ".dylib";
    #else
        libPath = libPath + ".so";
    #endif

    fs::path fullLibPath = buildPath / fs::path(libPath);
    if (!fileExists(fullLibPath)) {
        Out::error("Library file not found: %s", libPath.c_str());
        return false;
    }

    // Load a freshly-named COPY of the built library rather than the build
    // output directly. The dynamic loader identifies a library by its file
    // identity and dlclose()/FreeLibrary() are not guaranteed to unmap it, so
    // re-loading the same path after a rebuild can hand back the OLD code that
    // is still resident in the process (the reason a fresh rebuild only takes
    // effect after restarting the editor). On Windows LoadLibrary also keeps the
    // .dll locked, blocking the next build from overwriting it. A unique filename
    // per load forces the loader to map the new file every time.
    static std::atomic<unsigned long long> loadCounter{0};
    #ifdef _WIN32
        const std::string ext = ".dll";
    #elif defined(__APPLE__)
        const std::string ext = ".dylib";
    #else
        const std::string ext = ".so";
    #endif
    const std::string uniqueName = libName + "_" + std::to_string(loadCounter++) + "_" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ext;

    // Stage the copy in the build dir, falling back to the system temp dir if
    // that is not writable (disk full, permissions, read-only mount, ...). We
    // deliberately never fall back to loading fullLibPath directly: that path is
    // identity-cached by the loader and would silently re-run stale code, the
    // exact "build succeeded, Play runs old code" bug this staging prevents.
    std::error_code ec;
    std::vector<fs::path> stagingDirs;
    stagingDirs.push_back(buildPath / "hotreload");
    fs::path tempBase = fs::temp_directory_path(ec);
    if (!ec && !tempBase.empty()) {
        stagingDirs.push_back(tempBase / "doriax_hotreload");
    }

    fs::path copyPath;
    for (const fs::path& dir : stagingDirs) {
        std::error_code dirEc;
        fs::create_directories(dir, dirEc);

        // Best-effort sweep of copies left behind by previous sessions/crashes.
        // Still-mapped files fail to remove and are simply skipped.
        for (const auto& entry : fs::directory_iterator(dir, dirEc)) {
            std::error_code rmEc;
            fs::remove(entry.path(), rmEc);
        }

        fs::path candidate = dir / uniqueName;
        std::error_code cpEc;
        fs::copy_file(fullLibPath, candidate, fs::copy_options::overwrite_existing, cpEc);
        if (!cpEc) {
            copyPath = candidate;
            break;
        }
        Out::warning("Could not stage hot-reload copy in %s (%s)", dir.string().c_str(), cpEc.message().c_str());
    }

    if (copyPath.empty()) {
        Out::error("Failed to stage a loadable copy of the script library; aborting to avoid running stale code");
        return false;
    }

    libHandle = loadSharedLibrary(copyPath.string());
    if (!libHandle) {
        std::error_code rmEc;
        fs::remove(copyPath, rmEc);
        return false;
    }

    loadedLibCopy = copyPath;
    Out::info("Successfully connected to library");
    return true;
}

void editor::Conector::disconnect(){
    if (libHandle) {
        unloadSharedLibrary(libHandle);
        libHandle = nullptr;

        Out::info("Disconnected from library successfully");
    }

    if (!loadedLibCopy.empty()) {
        // Safe to unlink even if the OS kept the library mapped: on POSIX the
        // inode outlives the directory entry, and on Windows this is best-effort.
        std::error_code ec;
        fs::remove(loadedLibCopy, ec);
        loadedLibCopy.clear();
    }
}

void editor::Conector::init(Scene* scene){
    if (!libHandle) {
        Out::error("Cannot execute: Not connected to library");
        return;
    }

    using InitScriptsFunc = void (*)(Scene*);
    #ifdef _WIN32
        InitScriptsFunc initScriptsFn = reinterpret_cast<InitScriptsFunc>(GetProcAddress(static_cast<HMODULE>(libHandle), "initScripts"));
        if (!initScriptsFn) {
            Out::error("Failed to find function 'initScripts' in the library (Error code: %lu)", GetLastError());
        }
    #else
        dlerror(); // clear any existing error
        InitScriptsFunc initScriptsFn = reinterpret_cast<InitScriptsFunc>(dlsym(libHandle, "initScripts"));
        const char* err = dlerror();
        if (err) {
            Out::error("Failed to find function 'initScripts' in the library (Error: %s)", err);
            initScriptsFn = nullptr;
        }
    #endif

    if (initScriptsFn) {
        try {
            initScriptsFn(scene);
        } catch (const std::exception& e) {
            Out::error("Exception in initScripts(): %s", e.what());
        } catch (...) {
            Out::error("Unknown exception in initScripts()");
        }
    }
}

void editor::Conector::cleanup(Scene* scene){
    if (!libHandle) {
        Out::error("Cannot cleanup: Not connected to library");
        return;
    }

    using CleanupScriptsFunc = void (*)(Scene*);
    #ifdef _WIN32
        CleanupScriptsFunc cleanupScriptsFn = reinterpret_cast<CleanupScriptsFunc>(GetProcAddress(static_cast<HMODULE>(libHandle), "cleanupScripts"));
        if (!cleanupScriptsFn) {
            Out::error("Failed to find function 'cleanupScripts' in the library (Error code: %lu)", GetLastError());
        }
    #else
        dlerror(); // clear any existing error
        CleanupScriptsFunc cleanupScriptsFn = reinterpret_cast<CleanupScriptsFunc>(dlsym(libHandle, "cleanupScripts"));
        const char* err = dlerror();
        if (err) {
            Out::error("Failed to find function 'cleanupScripts' in the library (Error: %s)", err);
            cleanupScriptsFn = nullptr;
        }
    #endif

    if (cleanupScriptsFn) {
        try {
            cleanupScriptsFn(scene);
        } catch (const std::exception& e) {
            Out::error("Exception in cleanupScripts(): %s", e.what());
        } catch (...) {
            Out::error("Unknown exception in cleanupScripts()");
        }
    } else {
        Out::warning("Cleanup function not found in library");
    }
}

bool editor::Conector::isLibraryConnected() const {
    return libHandle != nullptr;
}