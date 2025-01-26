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
        
        // Main task loop - just keep alive
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    void rx_data(const uint8_t data[kIpcMessageBufferDataSize]) {
        const auto* cam_msg = reinterpret_cast<const CameraDataMessage*>(data);
        
        if (cam_msg->type == AppMessageType::kCameraData) {
            // Create new CameraData instance
            CameraData camera_data;
            camera_data.width = cam_msg->width;
            camera_data.height = cam_msg->height;
            camera_data.format = cam_msg->format;
            camera_data.timestamp = cam_msg->timestamp;
            
            // Copy image data to shared_ptr vector
            camera_data.image_data->resize(cam_msg->data_size);
            memcpy(camera_data.image_data->data(),
                cam_msg->data,
                cam_msg->data_size);
            
            // Place in queue
            xQueueOverwrite(g_ipc_camera_queue_m7, &camera_data);
            
            if (!first_message_rx_flag) {
                MulticoreMutexLock lock(0);
                printf("M7 IPC: First message received (data size: %u)\r\n", cam_msg->data_size);
                first_message_rx_flag = true;
            }
        } else {
            MulticoreMutexLock lock(0);
            printf("M7 IPC: Unknown message type received: %d\r\n", 
                static_cast<int>(cam_msg->type));
        }
    }

} // namespace coralmicro