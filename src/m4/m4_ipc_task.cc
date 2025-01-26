#include "m4/m4_ipc_task.hh"

namespace coralmicro {

    bool first_message_tx_flag = false; // Flag to indicate first message sent

    void print_start_message() {
        MulticoreMutexLock lock(0);
        printf("M4 IPC task starting...\r\n");
    }

    void m4_ipc_task(void* parameters) {
        (void)parameters;
        
        print_start_message();

        while (true) {
            tx_data();

            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    void tx_data() {
        CameraData camera_data;
        
        if (xQueuePeek(g_camera_queue_m4, &camera_data, 0) == pdTRUE) {
            size_t data_size = camera_data.image_data->size();
            
            // Prepare IPC message
            IpcMessage camera_data_msg{};
            camera_data_msg.type = IpcMessageType::kApp;
            auto* app_msg = reinterpret_cast<AppMessage*>(&camera_data_msg.message.data);
            app_msg->type = AppMessageType::kCameraData;
            
            // Fill in the camera data
            auto* ipc_data = reinterpret_cast<CameraDataIPC*>(app_msg->data);
            ipc_data->width = camera_data.width;
            ipc_data->height = camera_data.height;
            ipc_data->format = camera_data.format;
            ipc_data->timestamp = camera_data.timestamp;
            ipc_data->data_size = std::min(data_size, sizeof(ipc_data->data));
            
            // Copy the actual image data
            memcpy(ipc_data->data, 
                camera_data.image_data->data(),
                ipc_data->data_size);
                
            // Send the message
            IpcM4::GetSingleton()->SendMessage(camera_data_msg);

            if (!first_message_tx_flag) {
                MulticoreMutexLock lock(0);
                printf("M4 IPC: First message sent\r\n");
                first_message_tx_flag = true;
            }
        }
    }
} // namespace coralmicro