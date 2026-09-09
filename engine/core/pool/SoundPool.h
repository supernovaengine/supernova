// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef SOUNDPOOL_H
#define SOUNDPOOL_H

#include "Engine.h"

#include <atomic>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace SoLoud{
    class Wav;
}

namespace doriax{

    typedef std::map<std::string, std::shared_ptr<SoLoud::Wav>> sounds_t;

    struct SoundLoadResult {
        std::string id;
        ResourceLoadState state = ResourceLoadState::NotStarted;
        std::string errorMessage;
        std::shared_ptr<SoLoud::Wav> data = nullptr;

    };

    class DORIAX_API SoundPool{
    private:
        static sounds_t& getMap();

        static std::unordered_map<std::string, std::future<std::shared_ptr<SoLoud::Wav>>> pendingBuilds;
        static std::mutex cacheMutex;
        static std::atomic<bool> shutdownRequested;

        static std::shared_ptr<SoLoud::Wav> loadSoundInternal(const std::string& id, const std::string& filename, bool trackProgress);
        static std::string getSoundDisplayName(const std::string& path);

    public:
        static SoundLoadResult loadFromFile(const std::string& id, const std::string& filename);

        static void requestShutdown();

        static void remove(const std::string& id);

        // necessary for engine shutdown
        static void clear();
        // removes only entries no one else references (safe while other scenes are alive)
        static void clearUnused();
    };
}

#endif /* SOUNDPOOL_H */