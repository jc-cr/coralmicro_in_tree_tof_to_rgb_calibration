// camera_task.hh

#pragma once

#include "libs/camera/camera.h"
#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/task.h"

// Need below lib for I2C5Handle()
#include "libs/base/main_freertos_m4.h"

#include "m4/m4_queues.hh"
#include "libs/base/mutex.h"

#include "libs/base/ipc_m4.h"

#include "ipc_message.hh"
#include "shared_memory.hh"

namespace coralmicro{

    struct CameraConfig{
        static constexpr uint32_t kWidth = 324;
        static constexpr uint32_t kHeight = 324;
        static constexpr CameraFormat kFormat = CameraFormat::kRgb;
        static constexpr CameraFilterMethod filter = CameraFilterMethod::kBilinear;
        static constexpr CameraRotation rotation = CameraRotation::k270;
        static constexpr bool auto_white_balance = false;
    };

    void camera_task(void* parameters);
}