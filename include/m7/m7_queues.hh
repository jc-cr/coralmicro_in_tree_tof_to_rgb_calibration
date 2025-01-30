// m7_queues.hh
#pragma once

#include <vector>
#include <memory>

#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/queue.h"

// Camera
#include "libs/camera/camera.h"

// TOF
extern "C" {
#include "vl53l8cx_api.h"
}


namespace coralmicro {

    struct CameraData {
        uint32_t width;
        uint32_t height;
        CameraFormat format;
        TickType_t timestamp;
        
        // Place image buffer in non-cached memory for DMA
        alignas(32) std::vector<uint8_t, FreeRTOS::NoCacheAllocator<uint8_t>> image_data;
    };


    struct TofData {
        VL53L8CX_ResultsData tof_results;
        TickType_t timestamp;
    };

    // Queue handles
    inline QueueHandle_t g_tof_queue_m7;      // Latest TOF frame
    inline QueueHandle_t g_camera_queue_m7;   // Latest camera frame


    // Queue creation
    inline bool InitQueues() {
        g_tof_queue_m7 = xQueueCreate(1, sizeof(TofData));
        g_camera_queue_m7 = xQueueCreate(1, sizeof(CameraData));
        
        return (g_tof_queue_m7 != nullptr && g_camera_queue_m7 != nullptr);
    }

    // Queue cleanup
    inline void CleanupQueues() {
        if (g_tof_queue_m7) vQueueDelete(g_tof_queue_m7);
        if (g_camera_queue_m7) vQueueDelete(g_camera_queue_m7);
    }

}