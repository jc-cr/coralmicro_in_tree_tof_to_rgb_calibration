#include "m7/m7_ipc_task.hh"

namespace coralmicro {

    bool first_message_rx_flag = false; // Flag to indicate first message received

    static void print_start_message() {
        MulticoreMutexLock lock(0);
        printf("M7 IPC task starting...\r\n");
    }


    void m7_ipc_task(void* parameters) {
        (void)parameters;
        
        print_start_message(); 

        // Set the task's handler as the global handler
        g_ipc_task_handler = rx_data;
        
        // Main task loop
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

        void rx_data(const uint8_t data[kIpcMessageBufferDataSize]) {
            const auto* msg = reinterpret_cast<const AppMessage*>(data);
            
            switch (msg->type) {
                case AppMessageType::kCameraData: {
                    const auto* ipc_data = reinterpret_cast<const CameraDataIPC*>(msg->data);
                    
                    // Create new CameraData instance
                    CameraData camera_data;
                    camera_data.width = ipc_data->width;
                    camera_data.height = ipc_data->height;
                    camera_data.format = ipc_data->format;
                    camera_data.timestamp = ipc_data->timestamp;
                    
                    // Copy image data to shared_ptr vector
                    camera_data.image_data->resize(ipc_data->data_size);
                    memcpy(camera_data.image_data->data(),
                        ipc_data->data,
                        ipc_data->data_size);
                    
                    // Place in queue
                    xQueueOverwrite(g_ipc_camera_queue_m7, &camera_data);

                    if (!first_message_rx_flag) {
                        MulticoreMutexLock lock(0);
                        printf("M7 IPC: First message received\r\n");
                        first_message_rx_flag = true;
                    }
                    break;
                }
                
                default:
                    printf("M7 IPC: Unknown message type received\r\n");
                    break;
            }
        }

} // namespace coralmicro