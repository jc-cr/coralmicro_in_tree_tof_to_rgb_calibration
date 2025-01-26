#include "m7/m7_ipc_task.hh"

namespace coralmicro {

void m7_ipc_task(void* parameters) {
    (void)parameters;
    
    printf("M7 IPC task starting...\r\n");
    
    // Initialize IPC
    auto* ipc = IpcM7::GetSingleton();
    // Register message handler
    ipc->RegisterAppMessageHandler(rx_data);
    
    // Main task loop
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void rx_data(const uint8_t data[kIpcMessageBufferDataSize]) {
    const auto* msg = reinterpret_cast<const AppMessage*>(data);
    switch (msg->type) {

        case AppMessageType::kCameraData: {
            const auto* camera_data = reinterpret_cast<const CameraData*>(msg->data);

            // Place in inference input queue
            xQueueOverwrite(g_ipc_camera_queue_m7, camera_data);

            break;
        }

        default:
            printf("M7 IPC: Unknown message type received\r\n");
            break;
    }
}

} // namespace coralmicro