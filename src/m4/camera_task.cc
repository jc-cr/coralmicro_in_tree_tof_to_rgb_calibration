// camera_task.cc
#include "m4/camera_task.hh"

namespace coralmicro {

    bool first_frame_flag = false;

    void print_start_message() {
        MulticoreMutexLock lock(0);
        printf("Camera task starting...\r\n");
    }

    void print_ok_message() {
        MulticoreMutexLock lock(0);
        printf("Camera task started successfully\r\n");
    }

    void camera_task(void* parameters) {
        (void)parameters;
        
        print_start_message();
        // Initialize camera
        CameraTask::GetSingleton()->Init(I2C5Handle());
        CameraTask::GetSingleton()->SetPower(true);
        CameraTask::GetSingleton()->Enable(CameraMode::kStreaming);
        print_ok_message();
        auto& shared = SharedMemory::GetInstance();
        
        while (true) {
            CameraFrameFormat fmt{
                CameraConfig::kFormat,
                CameraConfig::filter,
                CameraConfig::rotation,
                CameraConfig::kWidth,
                CameraConfig::kHeight,
                false,
                shared.capture_frame.data,  // Always write to capture buffer
                CameraConfig::auto_white_balance
            };

            if (CameraTask::GetSingleton()->GetFrame({fmt})) {
                // Update metadata
                shared.capture_frame.width = CameraConfig::kWidth;
                shared.capture_frame.height = CameraConfig::kHeight;
                shared.capture_frame.format = CameraConfig::kFormat;
                shared.capture_frame.timestamp = xTaskGetTickCount();
                shared.capture_frame.data_size = CameraConfig::kWidth * 
                    CameraConfig::kHeight * CameraFormatBpp(CameraConfig::kFormat);

                // Only swap if M7 is ready for new frame
                MulticoreMutexLock lock(kSharedMemoryMutex);
                if (!shared.new_frame_ready) {
                    // Set flag and swap before notification
                    shared.new_frame_ready = true;
                    std::swap(shared.capture_frame, shared.process_frame);

                    // Send notification
                    IpcMessage msg{};
                    msg.type = IpcMessageType::kApp;
                    auto* notify = reinterpret_cast<NotificationMessage*>(&msg.message.data);
                    notify->type = AppMessageType::kNewFrame;
                    IpcM4::GetSingleton()->SendMessage(msg);

                    if (!first_frame_flag) {
                        MulticoreMutexLock print_lock(0);
                        printf("Camera: First frame captured and sent to M7 (size: %u)\r\n", 
                            shared.capture_frame.data_size);
                        first_frame_flag = true;
                    }
                }
            }
            
            vTaskDelay(pdMS_TO_TICKS(33));  // ~30fps
        }
    }


}