// m4_ipc_task.hh
#pragma once

#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/task.h"
#include "libs/base/ipc_m4.h"

#include "m4/m4_queues.hh"
#include "ipc_message.hh"

#include "libs/base/mutex.h"

namespace coralmicro {
    void m4_ipc_task(void* parameters);
    void tx_data();
}