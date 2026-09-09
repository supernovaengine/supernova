// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "util/TerrainMapFileWriter.h"

#include "Out.h"
#include "util/PngWriter.h"
#include "stb_image_write.h"

#include <cstdio>
#include <utility>

using namespace doriax::editor;

namespace fs = std::filesystem;

TerrainMapFileWriter& TerrainMapFileWriter::get(){
    static TerrainMapFileWriter instance;
    return instance;
}

void TerrainMapFileWriter::enqueue(const fs::path& path, int width, int height, int channels, int bytesPerChannel, std::vector<unsigned char> pixels){
    std::unique_lock<std::mutex> lock(mutex);
    failedJobs.erase(path.string());  // superseded by newer pixels
    Job& job = jobs[path.string()];
    job.width = width;
    job.height = height;
    job.channels = channels;
    job.bytesPerChannel = bytesPerChannel;
    job.pixels = std::move(pixels);
    startWorkerLocked();
    condition.notify_all();
}

void TerrainMapFileWriter::flushPath(const std::string& path){
    std::unique_lock<std::mutex> lock(mutex);
    retryFailedLocked(path);
    condition.wait(lock, [&]{ return jobs.find(path) == jobs.end() && writingPath != path; });
}

bool TerrainMapFileWriter::flushAll(){
    std::unique_lock<std::mutex> lock(mutex);
    for (auto& [path, job] : failedJobs){
        if (jobs.find(path) == jobs.end()){
            jobs[path] = std::move(job);
        }
    }
    failedJobs.clear();
    if (!jobs.empty()){
        startWorkerLocked();
        condition.notify_all();
    }
    condition.wait(lock, [&]{ return jobs.empty() && writingPath.empty(); });
    return failedJobs.empty();
}

size_t TerrainMapFileWriter::failedCount(){
    std::unique_lock<std::mutex> lock(mutex);
    return failedJobs.size();
}

TerrainMapFileWriter::~TerrainMapFileWriter(){
    std::thread joining;
    {
        std::unique_lock<std::mutex> lock(mutex);
        stopRequested = true;
        joining = std::move(worker);
    }
    condition.notify_all();
    if (joining.joinable()){
        joining.join();
    }

    // Last chance for writes that kept failing: retry synchronously now that
    // the worker is gone — a since-resolved disk problem (freed space, fixed
    // permissions) still persists the sculpt. Out may already be torn down
    // this late in shutdown, so report irrecoverable losses to stderr.
    for (const auto& [path, job] : failedJobs){
        if (!writeJob(path, job)){
            std::fprintf(stderr, "[ERROR] Terrain map lost, could not be written: %s\n", path.c_str());
        }
    }
}

bool TerrainMapFileWriter::writeJob(const std::string& path, const Job& job){
    if (job.bytesPerChannel >= 2){
        return writeGray16Png(fs::path(path), job.width, job.height, job.pixels.data(), job.pixels.size());
    }
    return stbi_write_png(path.c_str(), job.width, job.height, job.channels, job.pixels.data(), job.width * job.channels) != 0;
}

void TerrainMapFileWriter::startWorkerLocked(){
    if (!workerRunning){
        if (worker.joinable()){
            worker.join();
        }
        workerRunning = true;
        stopRequested = false;
        worker = std::thread(&TerrainMapFileWriter::run, this);
    }
}

void TerrainMapFileWriter::retryFailedLocked(const std::string& path){
    auto failed = failedJobs.find(path);
    if (failed == failedJobs.end()){
        return;
    }
    if (jobs.find(path) == jobs.end()){
        jobs[path] = std::move(failed->second);
    }
    failedJobs.erase(failed);
    startWorkerLocked();
    condition.notify_all();
}

void TerrainMapFileWriter::run(){
    std::unique_lock<std::mutex> lock(mutex);
    while (true){
        condition.wait(lock, [&]{ return !jobs.empty() || stopRequested; });
        if (jobs.empty()){
            if (stopRequested){
                break;
            }
            continue;
        }

        auto it = jobs.begin();
        const std::string path = it->first;
        Job job = std::move(it->second);
        jobs.erase(it);
        writingPath = path;
        lock.unlock();

        const bool written = writeJob(path, job);
        if (!written){
            // Out marshals to the main thread when called from a worker.
            Out::error("Failed to write terrain map %s — the edit is kept in memory and the write will be retried on the next save", path.c_str());
        }

        lock.lock();
        if (!written && jobs.find(path) == jobs.end()){
            failedJobs[path] = std::move(job);
        }
        writingPath.clear();
        condition.notify_all();
    }
    workerRunning = false;
    condition.notify_all();
}
