#pragma once

#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/queue.h"
#include "libs/camera/camera.h"
#include <vector>
#include <memory>



extern "C" {
#include "vl53l8cx_api.h"
}

namespace coralmicro {

// Structure for camera data using pointer to avoid queue copies
struct CameraData {
    uint32_t width;
    uint32_t height;
    CameraFormat format;
    TickType_t timestamp;
    std::shared_ptr<std::vector<uint8_t>> image_data;

    CameraData() : image_data(std::make_shared<std::vector<uint8_t>>()) {}
};

// Queue handles
inline QueueHandle_t g_tof_queue;      // Latest TOF frame
inline QueueHandle_t g_camera_queue;    // Latest camera frame

// Queue creation with proper initialization order
inline bool InitQueues() {
    // Create queues before starting tasks
    g_tof_queue = xQueueCreate(1, sizeof(VL53L8CX_ResultsData));
    if (!g_tof_queue) {
        printf("Failed to create TOF queue\r\n");
        return false;
    }

    g_camera_queue = xQueueCreate(1, sizeof(CameraData));
    if (!g_camera_queue) {
        printf("Failed to create camera queue\r\n");
        vQueueDelete(g_tof_queue);
        return false;
    }

    return true;
}

// Queue cleanup
inline void CleanupQueues() {
    if (g_tof_queue) vQueueDelete(g_tof_queue);
    if (g_camera_queue) vQueueDelete(g_camera_queue);
}

} // namespace coralmicro