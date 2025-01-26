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


#include "ipc_message.hh"


namespace coralmicro {

    // Data structures

    struct CameraData {
        uint32_t width;
        uint32_t height;
        CameraFormat format;
        TickType_t timestamp;
        std::shared_ptr<std::vector<uint8_t>> image_data;

        CameraData() : image_data(std::make_shared<std::vector<uint8_t>>()) {}
    };

    struct CameraDataIPC {
        uint32_t width;
        uint32_t height;
        CameraFormat format;
        TickType_t timestamp;
        uint32_t data_size;
        uint8_t data[kIpcMessageBufferDataSize - sizeof(AppMessageType) - 
                    sizeof(uint32_t) * 4 - sizeof(CameraFormat) - sizeof(TickType_t)];
    } __attribute__((packed));

    static_assert(sizeof(CameraDataIPC) <= kIpcMessageBufferDataSize - sizeof(AppMessageType),
                "CameraDataIPC too large for IPC buffer");

    inline std::function<void(const uint8_t data[kIpcMessageBufferDataSize])> g_ipc_task_handler;
    
    struct TofData {
        VL53L8CX_ResultsData tof_results;
        TickType_t timestamp;
    };

    // Queue handles
    inline QueueHandle_t g_tof_queue_m7;      // Latest TOF frame
    inline QueueHandle_t g_ipc_camera_queue_m7;  // Latest camera frame


    // Queue creation
    inline bool InitQueues() {


        g_tof_queue_m7 = xQueueCreate(1, sizeof(TofData));
        g_ipc_camera_queue_m7 = xQueueCreate(1, sizeof(CameraData));
        
        return (g_tof_queue_m7 != nullptr && g_ipc_camera_queue_m7 != nullptr);
    }

    // Queue cleanup
    inline void CleanupQueues() {
        if (g_tof_queue_m7) vQueueDelete(g_tof_queue_m7);
        if (g_ipc_camera_queue_m7) vQueueDelete(g_ipc_camera_queue_m7);
    }

}