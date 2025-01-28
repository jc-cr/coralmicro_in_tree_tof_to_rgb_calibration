// shared_memory.hh
#pragma once
#include <atomic>
#include "libs/camera/camera.h"

namespace coralmicro {

struct CameraFrame {
    uint32_t width;
    uint32_t height;
    CameraFormat format;
    TickType_t timestamp;
    uint32_t data_size;
    uint8_t data[324 * 324 * 3];  // Max size for RGB 324x324
};

struct SharedMemory {
    CameraFrame capture_frame;   // M4 writes here
    CameraFrame process_frame;   // M7 reads from here
    std::atomic<bool> new_frame_ready{false};

    static SharedMemory& GetInstance() {
        static SharedMemory instance __attribute__((section(".shared_memory")));
        return instance;
    }
};

// Single mutex protects frame swapping
static constexpr int kSharedMemoryMutex = 1;

} // namespace coralmicro