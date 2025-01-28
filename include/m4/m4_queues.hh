// m4_queues.hh
#pragma once


#include <vector>
#include <memory>

#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/queue.h"
#include "libs/camera/camera.h"


namespace coralmicro {

    // Queue data structures

    // Queue creation
    inline bool InitQueues() {

        return true;
    }

    // Queue cleanup
    inline void CleanupQueues() {

    }

}