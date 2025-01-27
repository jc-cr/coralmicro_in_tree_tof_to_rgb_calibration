// m4_queues.hh
#pragma once


#include <vector>
#include <memory>

#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/queue.h"
#include "libs/camera/camera.h"

#include "ipc_message.hh"


namespace coralmicro {

    // Queue data structures
    struct CameraData {
        uint32_t width;
        uint32_t height;
        CameraFormat format;
        TickType_t timestamp;
        uint32_t data_size;
        uint8_t data[324 * 324 * 3];  // Max size for RGB 324x324
    };

    // Queue handles
    inline QueueHandle_t g_camera_queue_m4;      // Latest camera frame

    // Queue creation
    inline bool InitQueues() {
        g_camera_queue_m4 = xQueueCreate(1, sizeof(CameraData));

        return (g_camera_queue_m4 != nullptr);
    }

    // Queue cleanup
    inline void CleanupQueues() {
        if (g_camera_queue_m4) vQueueDelete(g_camera_queue_m4);
    }

}