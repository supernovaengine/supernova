//
// (c) 2024 Eduardo Doria.
//

#include "TextureDataPool.h"

#include "Engine.h"
#include "Log.h"
#include "util/ResourceProgress.h"

#include <filesystem>

using namespace Supernova;

bool TextureDataPool::asyncLoading = false;
std::unordered_map<std::string, std::future<std::array<TextureData,6>>> TextureDataPool::pendingBuilds;
std::mutex TextureDataPool::cacheMutex;
std::atomic<bool> TextureDataPool::shutdownRequested{false};

texturesdata_t& TextureDataPool::getMap(){
    //To prevent similar problem of static init fiasco but on deinitialization
    //https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
    static texturesdata_t* map = new texturesdata_t();
    return *map;
};

std::shared_ptr<std::array<TextureData,6>> TextureDataPool::get(const std::string& id){
	auto& shared = getMap()[id];

	if (shared.use_count() > 0){
		return shared;
	}

	return nullptr;
}

std::shared_ptr<std::array<TextureData,6>> TextureDataPool::get(const std::string& id, std::array<TextureData,6> data){
	auto& shared = getMap()[id];

	if (shared.use_count() > 0){
		return shared;
	}

	shared = std::make_shared<std::array<TextureData,6>>(data);

	return shared;
}

std::shared_ptr<std::array<TextureData,6>> TextureDataPool::loadFromFile(const std::string& id, const std::array<std::string, 6>& paths, size_t numFaces){
    auto& shared = getMap()[id];

    if (shared && shared.use_count() > 0) {
        return shared;
    }

    if (asyncLoading) {
        std::lock_guard<std::mutex> lock(cacheMutex);

        if (shutdownRequested.load()) {
            throw std::runtime_error("Shutdown requested");
        }

        // Check if already building
        auto it = pendingBuilds.find(id);
        if (it != pendingBuilds.end()) {
            auto& future = it->second;
            if (future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                // Build finished, move to cache
                try {
                    std::array<TextureData,6> data = future.get();
                    shared = std::make_shared<std::array<TextureData,6>>(data);
                    pendingBuilds.erase(it);
                    ResourceProgress::completeBuild(std::hash<std::string>{}(id));
                    return shared;
                } catch (const std::exception& e) {
                    pendingBuilds.erase(it);
                    ResourceProgress::failBuild(std::hash<std::string>{}(id));
                    Log::error("Failed to load texture: %s", e.what());
                    return nullptr;
                }
            } else if (future.valid()) {
                // Still building
                return nullptr;
            } else {
                // Invalid future, remove it
                pendingBuilds.erase(it);
            }
        }

        if (shutdownRequested.load()) {
            throw std::runtime_error("Shutdown requested");
        }

        // Start new async build
        std::string textureName = getTextureDisplayName(id);
        uint64_t buildId = std::hash<std::string>{}(id);

        ResourceProgress::startBuild(buildId, ResourceType::Texture, textureName);

        pendingBuilds[id] = std::async(std::launch::async, [id, paths, numFaces, buildId]() {
            return loadTextureInternal(id, paths, numFaces);
        });

    } else {

        std::array<TextureData,6> data = loadTextureInternal(id, paths, numFaces);

        shared = std::make_shared<std::array<TextureData,6>>(data);

        return shared;

    }

    return nullptr; // Will be ready on next call
}

std::array<TextureData,6> TextureDataPool::loadTextureInternal(const std::string& id, const std::array<std::string, 6>& paths, size_t numFaces) {
    if (shutdownRequested.load()) {
        throw std::runtime_error("Shutdown requested");
    }

    uint64_t buildId = std::hash<std::string>{}(id);
    if (asyncLoading) {
        ResourceProgress::updateProgress(buildId, 0.1f);  // Starting
    }

    std::array<TextureData,6> data;

    for (size_t f = 0; f < numFaces; f++) {
        if (shutdownRequested.load()) {
            throw std::runtime_error("Shutdown requested");
        }

        if (paths[f].empty()) {
            throw std::runtime_error("Texture is missing texture for face " + std::to_string(f));
        }

        bool success = data[f].loadTextureFromFile(paths[f].c_str());
        if (!success) {
            throw std::runtime_error("Failed to load texture face " + std::to_string(f) + " from file: " + paths[f]);
        }

        // Apply texture strategy
        if (Engine::getTextureStrategy() == TextureStrategy::FIT) {
            data[f].fitPowerOfTwo();
        } else if (Engine::getTextureStrategy() == TextureStrategy::RESIZE) {
            data[f].resizePowerOfTwo();
        }

        if (asyncLoading) {
            ResourceProgress::updateProgress(buildId, 0.1f + (0.8f * (f + 1) / static_cast<float>(numFaces)));
        }
    }

    if (shutdownRequested.load()) {
        throw std::runtime_error("Shutdown requested");
    }

    if (asyncLoading) {
        ResourceProgress::updateProgress(buildId, 1.0f);  // Completing
    }

    return data;
}

std::string TextureDataPool::getTextureDisplayName(const std::string& path) {
    std::filesystem::path filePath(path);
    return filePath.filename().string();
}

void TextureDataPool::setAsyncLoading(bool enable){
    asyncLoading = enable;
}

bool TextureDataPool::isAsyncLoading(){
    return asyncLoading;
}

void TextureDataPool::requestShutdown() {
    std::lock_guard<std::mutex> lock(cacheMutex);
    shutdownRequested = true;

    // Wait for all pending builds to complete
    for (auto& [id, future] : pendingBuilds) {
        if (future.valid()) {
            future.wait(); // Wait for completion
        }
    }
    pendingBuilds.clear();
}

void TextureDataPool::remove(const std::string& id){
	if (getMap().count(id)){
		auto& shared = getMap()[id];
		if (shared.use_count() <= 1){
			getMap().erase(id);
		}
	}else{
		if (Engine::isViewLoaded()){
			Log::debug("Trying to destroy a non existent texture data: %s", id.c_str());
		}
	}
}

void TextureDataPool::clear(){
	if (asyncLoading){
		std::lock_guard<std::mutex> lock(cacheMutex);
		// Clear pending builds
		for (auto& [id, future] : pendingBuilds) {
			if (future.valid()) {
				future.wait(); // Wait for completion before clearing
			}
		}
		pendingBuilds.clear();
	}
	getMap().clear();
}
