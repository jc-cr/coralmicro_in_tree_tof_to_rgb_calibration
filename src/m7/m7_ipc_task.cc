// m7_ipc_task.cc
#include "m7/m7_ipc_task.hh"

namespace coralmicro {

bool first_message_rx_flag = false;

static void print_start_message() {
    MulticoreMutexLock lock(0);
    printf("M7 IPC task starting...\r\n");
}

void m7_ipc_task(void* parameters) {
    (void)parameters;
    print_start_message();
    
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void rx_data(const uint8_t data[kIpcMessageBufferDataSize]) {
    const auto* msg = reinterpret_cast<const NotificationMessage*>(data);
    
    if (msg->type == AppMessageType::kNewFrame) {
        if (!first_message_rx_flag) {
            MulticoreMutexLock lock(0);
            printf("M7 IPC: First frame notification received\r\n");
            first_message_rx_flag = true;
        }
        // Frame is ready in shared memory
        // RPC task will handle clearing new_frame_ready flag
    }
}

}