// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <condition_variable>
#include <cstddef>
#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace doriax::editor {

// Background writer for terrain map files. Brush strokes only touch the in-memory
// map (and the GPU copy); the PNG encode + disk write of the full map — slow enough
// on large 16-bit heightmaps to hitch the editor on every stroke end — happens
// here, off the UI thread. Jobs are latest-wins per path, so rapid strokes collapse
// into a single write. The in-memory TextureDataPool entry is seeded before any
// write is queued, so the running editor never depends on the file being current;
// readers that do go to disk must call flushPath() first.
//
// A failed write keeps its pixels in failedJobs and is retried on every flush
// (scene save, map GC, disk reads, window teardown), so transient errors heal and
// persistent ones keep being reported instead of silently losing the sculpt at
// the next restart. A flush that retries and fails again still returns — the job
// simply stays queued for the next flush.
//
// Lives in the editor utility layer (not in any feature/UI module) so the terrain
// editor and the AI terrain capability can share one coordinated write queue.
class TerrainMapFileWriter {
public:
    static TerrainMapFileWriter& get();

    // Queue a full-map write; latest pixels win per path. Takes ownership of pixels.
    void enqueue(const std::filesystem::path& path, int width, int height, int channels, int bytesPerChannel, std::vector<unsigned char> pixels);

    // Blocks until a pending write for this path (if any) has been attempted;
    // previously failed writes get one retry first.
    void flushPath(const std::string& path);

    // Blocks until every queued write has been attempted; previously failed writes
    // get one retry first. Returns false when writes still failed — those jobs stay
    // queued for the next flush, and the caller should tell the user the data is not
    // durable yet.
    bool flushAll();

    // Number of writes currently in the failed state (for user-facing messages).
    size_t failedCount();

    ~TerrainMapFileWriter();

    TerrainMapFileWriter(const TerrainMapFileWriter&) = delete;
    TerrainMapFileWriter& operator=(const TerrainMapFileWriter&) = delete;

private:
    struct Job {
        int width = 0;
        int height = 0;
        int channels = 0;
        int bytesPerChannel = 1;
        std::vector<unsigned char> pixels;
    };

    TerrainMapFileWriter() = default;

    static bool writeJob(const std::string& path, const Job& job);
    void startWorkerLocked();
    void retryFailedLocked(const std::string& path);
    void run();

    std::mutex mutex;
    std::condition_variable condition;
    std::map<std::string, Job> jobs;
    std::map<std::string, Job> failedJobs;
    std::string writingPath;
    std::thread worker;
    bool workerRunning = false;
    bool stopRequested = false;
};

}
