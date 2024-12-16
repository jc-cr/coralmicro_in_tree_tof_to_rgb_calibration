// camera_task.hh

#pragma once

#include "libs/camera/camera.h"
#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/task.h"

#include "sensor_queues.hh"

namespace coralmicro{

    void camera_task(void* parameters);
}