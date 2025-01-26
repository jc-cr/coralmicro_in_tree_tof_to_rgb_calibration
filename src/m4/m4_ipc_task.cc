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
            
            // Cast directly to our camera message structure
            auto* cam_msg = reinterpret_cast<CameraDataMessage*>(&camera_data_msg.message.data);
            cam_msg->type = AppMessageType::kCameraData;
            cam_msg->width = camera_data.width;
            cam_msg->height = camera_data.height;
            cam_msg->format = camera_data.format;
            cam_msg->timestamp = camera_data.timestamp;
            cam_msg->data_size = std::min(data_size, sizeof(cam_msg->data));
            
            // Copy the actual image data
            memcpy(cam_msg->data, 
                camera_data.image_data->data(),
                cam_msg->data_size);
                    
            // Send the message
            IpcM4::GetSingleton()->SendMessage(camera_data_msg);
            
            if (!first_message_tx_flag) {
                MulticoreMutexLock lock(0);
                printf("M4 IPC: First message sent (data size: %u)\r\n", cam_msg->data_size);
                first_message_tx_flag = true;
            }
        }
    }
} // namespace coralmicro