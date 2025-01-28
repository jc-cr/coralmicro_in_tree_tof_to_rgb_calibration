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

    // Data structures

    struct TofData {
        VL53L8CX_ResultsData tof_results;
        TickType_t timestamp;
    };

    // Queue handles
    inline QueueHandle_t g_tof_queue_m7;      // Latest TOF frame


    // Queue creation
    inline bool InitQueues() {


        g_tof_queue_m7 = xQueueCreate(1, sizeof(TofData));
        
        return (g_tof_queue_m7 != nullptr);
    }

    // Queue cleanup
    inline void CleanupQueues() {
        if (g_tof_queue_m7) vQueueDelete(g_tof_queue_m7);
    }

}