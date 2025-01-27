// camera_task.cc
#include "m4/camera_task.hh"

namespace coralmicro {

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
        CameraTask::GetSingleton()->Init(I2C5Handle()); // Specific to M4 core
        CameraTask::GetSingleton()->SetPower(true);
        CameraTask::GetSingleton()->Enable(CameraMode::kStreaming);

        // Main task loop
        CameraData camera_data;
        camera_data.width = CameraConfig::kWidth;
        camera_data.height = CameraConfig::kHeight;
        camera_data.format = CameraConfig::kFormat;

        print_ok_message();

        while (true) {
            camera_data.timestamp = xTaskGetTickCount();
            camera_data.data_size = camera_data.width * camera_data.height * 
                CameraFormatBpp(camera_data.format);

            CameraFrameFormat fmt{
                camera_data.format,
                CameraConfig::filter,
                CameraConfig::rotation,
                static_cast<int>(camera_data.width),
                static_cast<int>(camera_data.height),
                false,
                camera_data.data,  // Direct use of the buffer
                CameraConfig::auto_white_balance
            };

            if (CameraTask::GetSingleton()->GetFrame({fmt})) {
                if (xQueueOverwrite(g_camera_queue_m4, &camera_data) != pdTRUE) {
                    MulticoreMutexLock lock(0);
                    printf("Failed to send camera data to queue\r\n");
                }
            } else {
                printf("Camera frame capture failed\r\n");
            }

            vTaskDelay(pdMS_TO_TICKS(33));  // ~30 FPS
        }
    }
}