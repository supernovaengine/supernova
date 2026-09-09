// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ResourceProgress.h"

#include "Log.h"
#include <algorithm>

using namespace doriax;

std::mutex& ResourceProgress::getProgressMutex(){
    static std::mutex* mutex = new std::mutex();
    return *mutex;
}

ResourceProgress::resource_builds_t& ResourceProgress::getActiveBuilds(){
    static resource_builds_t* builds = new resource_builds_t();
    return *builds;
}

uint64_t& ResourceProgress::getMostRecentBuildId(){
    static uint64_t* buildId = new uint64_t(0);
    return *buildId;
}

void ResourceProgress::startBuild(uint64_t id, ResourceType type, const std::string& name) {
    Log::debug("Loading started [%s]: %s", getResourceTypeName(type).c_str(), name.c_str());
    std::lock_guard<std::mutex> lock(getProgressMutex());
    auto& activeBuilds = getActiveBuilds();
    auto& mostRecentBuildId = getMostRecentBuildId();

    ResourceBuildInfo info;
    info.type = type;
    info.name = name;
    info.progress = 0.0f;
    info.isActive = true;
    info.startTime = std::chrono::steady_clock::now();

    activeBuilds[id] = info;
    mostRecentBuildId = id;
}

void ResourceProgress::updateProgress(uint64_t id, float progress) {
    std::lock_guard<std::mutex> lock(getProgressMutex());
    auto& activeBuilds = getActiveBuilds();

    auto it = activeBuilds.find(id);
    if (it != activeBuilds.end()) {
        it->second.progress = std::clamp(progress, 0.0f, 1.0f);
    }
}

void ResourceProgress::completeBuild(uint64_t id) {
    std::lock_guard<std::mutex> lock(getProgressMutex());
    auto& activeBuilds = getActiveBuilds();
    auto it = activeBuilds.find(id);
    if (it != activeBuilds.end()) {
        Log::debug("Loading completed [%s]: %s", getResourceTypeName(it->second.type).c_str(), it->second.name.c_str());
        activeBuilds.erase(it);
        return;
    }
    Log::debug("Loading completed [Resource]: <unknown>");
}

void ResourceProgress::failBuild(uint64_t id) {
    std::lock_guard<std::mutex> lock(getProgressMutex());
    auto& activeBuilds = getActiveBuilds();
    auto it = activeBuilds.find(id);
    if (it != activeBuilds.end()) {
        Log::debug("Loading failed [%s]: %s", getResourceTypeName(it->second.type).c_str(), it->second.name.c_str());
        activeBuilds.erase(it);
        return;
    }
    Log::debug("Loading failed [Resource]: <unknown>");
}

bool ResourceProgress::hasActiveBuilds() {
    std::lock_guard<std::mutex> lock(getProgressMutex());
    auto& activeBuilds = getActiveBuilds();
    return !activeBuilds.empty();
}

OverallBuildProgress ResourceProgress::getOverallProgress() {
    std::lock_guard<std::mutex> lock(getProgressMutex());
    auto& activeBuilds = getActiveBuilds();
    auto& mostRecentBuildId = getMostRecentBuildId();

    OverallBuildProgress overall;

    if (activeBuilds.empty()) {
        return overall; // All defaults to false/0
    }

    overall.hasActiveBuilds = true;
    overall.totalBuilds = static_cast<int>(activeBuilds.size());

    // Calculate total progress across all builds
    float totalProgress = 0.0f;
    for (const auto& [id, build] : activeBuilds) {
        totalProgress += build.progress;
    }
    overall.totalProgress = totalProgress / overall.totalBuilds;

    // Get information about the most recent build
    auto recentIt = activeBuilds.find(mostRecentBuildId);
    if (recentIt != activeBuilds.end()) {
        overall.currentBuildName = recentIt->second.name;
        overall.currentBuildType = recentIt->second.type;
    } else if (!activeBuilds.empty()) {
        // Fallback if most recent isn't found
        overall.currentBuildName = activeBuilds.begin()->second.name;
        overall.currentBuildType = activeBuilds.begin()->second.type;
    }

    return overall;
}

ResourceBuildInfo ResourceProgress::getCurrentBuild() {
    std::lock_guard<std::mutex> lock(getProgressMutex());
    auto& activeBuilds = getActiveBuilds();
    auto& mostRecentBuildId = getMostRecentBuildId();

    if (activeBuilds.empty()) {
        return {};
    }

    auto it = activeBuilds.find(mostRecentBuildId);
    if (it != activeBuilds.end()) {
        return it->second;
    }

    return activeBuilds.begin()->second;
}

int ResourceProgress::getActiveBuildCount() {
    std::lock_guard<std::mutex> lock(getProgressMutex());
    auto& activeBuilds = getActiveBuilds();
    return static_cast<int>(activeBuilds.size());
}

std::vector<ResourceBuildInfo> ResourceProgress::getAllActiveBuilds() {
    std::lock_guard<std::mutex> lock(getProgressMutex());
    auto& activeBuilds = getActiveBuilds();

    std::vector<ResourceBuildInfo> builds;
    builds.reserve(activeBuilds.size());

    for (const auto& [id, build] : activeBuilds) {
        builds.push_back(build);
    }

    // Sort by start time (most recent first) or by name for consistent ordering
    std::sort(builds.begin(), builds.end(), 
        [](const ResourceBuildInfo& a, const ResourceBuildInfo& b) {
            return a.startTime > b.startTime; // Most recent first
        });

    return builds;
}

std::string ResourceProgress::getResourceTypeName(ResourceType type) {
    switch (type) {
        case ResourceType::Shader: return "Shader";
        case ResourceType::Texture: return "Texture";
        case ResourceType::Model: return "Model";
        case ResourceType::Sound: return "Sound";
        default: return "Resource";
    }
}