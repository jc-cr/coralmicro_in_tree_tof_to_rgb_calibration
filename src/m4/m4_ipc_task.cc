#include "m4/m4_ipc_task.hh"

namespace coralmicro {

    void m4_ipc_task(void* parameters) {
        (void)parameters;
        
        printf("M4 IPC task starting... \r\n");

        while (true) {
            tx_data();

            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    void tx_data() {
        // Send pending IPC messages
        // Init data structures
        CameraData camera_data;

        if (xQueuePeek(g_camera_queue_m4, &camera_data, 0) == pdTRUE) {
            IpcMessage camera_data_msg{};
            camera_data_msg.type = IpcMessageType::kApp;
            auto* app_msg = reinterpret_cast<AppMessage*>(&camera_data_msg.message.data);
            app_msg->type = AppMessageType::kCameraData;
            memcpy(app_msg->data, &camera_data, sizeof(CameraData));

            IpcM4::GetSingleton()->SendMessage(camera_data_msg);
        }
    }
} // namespace coralmicro