// sensor_queues.hh
#pragma once

#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/queue.h"
#include "libs/camera/camera.h"
#include <vector>

extern "C" {
#include "vl53l8cx_api.h"
}

namespace coralmicro {

// Queue handles - single-item queues that always hold latest data
// Inline to avoid multiple definitions
inline QueueHandle_t g_tof_queue;      // Latest TOF frame
inline QueueHandle_t g_camera_queue;    // Latest camera frame

// Structure for camera data
struct CameraData {
    uint32_t width;
    uint32_t height;
    CameraFormat format;
    TickType_t timestamp;
    std::vector<uint8_t> image_data;
};

// Queue creation
inline bool InitQueues() {
    // Create queues with single item depth - overwrites with latest data
    g_tof_queue = xQueueCreate(1, sizeof(VL53L8CX_ResultsData));
    g_camera_queue = xQueueCreate(1, sizeof(CameraData));

    return (g_tof_queue != nullptr) && (g_camera_queue != nullptr);
}

// Queue cleanup
inline void CleanupQueues() {
    if (g_tof_queue) vQueueDelete(g_tof_queue);
    if (g_camera_queue) vQueueDelete(g_camera_queue);
}

} // namespace coralmicro