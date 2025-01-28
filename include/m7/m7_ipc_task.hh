// m7_ipc_task.hh
#pragma once

#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/task.h"

#include "libs/base/ipc_m7.h"
#include "libs/base/mutex.h"

#include "m7/m7_queues.hh"
#include "ipc_message.hh"
#include "shared_memory.hh"

namespace coralmicro{

    void m7_ipc_task(void* parameters);

}