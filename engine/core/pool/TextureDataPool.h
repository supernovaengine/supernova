//
// (c) 2024 Eduardo Doria.
//

#ifndef TEXTUREDATAPOOL_H
#define TEXTUREDATAPOOL_H

#include "render/TextureRender.h"
#include "texture/TextureData.h"
#include <map>
#include <memory>
#include <array>

#include <mutex>
#include <future>
#include <unordered_map>
#include <atomic>

namespace Supernova{

    typedef std::map< std::string, std::shared_ptr<std::array<TextureData,6>> > texturesdata_t;

    class SUPERNOVA_API TextureDataPool{
    private:
        static texturesdata_t& getMap();

        static bool asyncLoading;
        static std::unordered_map<std::string, std::future<std::array<TextureData,6>>> pendingBuilds;
        static std::mutex cacheMutex;
        static std::atomic<bool> shutdownRequested;

        static std::array<TextureData,6> loadTextureInternal(const std::string& id, const std::array<std::string, 6>& paths, size_t numFaces);
        static std::string getTextureDisplayName(const std::string& path);

    public:
        static std::shared_ptr<std::array<TextureData,6>> get(const std::string& id);
        static std::shared_ptr<std::array<TextureData,6>> get(const std::string& id, std::array<TextureData,6> data);

        // Loading methods
        static std::shared_ptr<std::array<TextureData,6>> loadFromFile(const std::string& id, const std::array<std::string, 6>& paths, size_t numFaces);

        static void setAsyncLoading(bool enable);
        static bool isAsyncLoading();

        static void requestShutdown();

        static void remove(const std::string& id);
        static void clear();
    };
}

#endif /* TEXTUREDATAPOOL_H */
