// camera_task.cc
#include "m4/camera_task.hh"

namespace coralmicro {

    void camera_task(void* parameters) {
        (void)parameters;


        printf("Camera task starting...\r\n");

        // Initialize camera
        CameraTask::GetSingleton()->Init(I2C5Handle()); // Specific to M4 core
        CameraTask::GetSingleton()->SetPower(true);
        CameraTask::GetSingleton()->Enable(CameraMode::kStreaming);

        // Main task loop
        CameraData camera_data;
        camera_data.width = CameraConfig::kWidth;
        camera_data.height = CameraConfig::kHeight;
        camera_data.format = CameraConfig::kFormat;

        printf("Camera task running...\r\n");

        while (true) {
            // Create new camera data instance
            camera_data.timestamp = xTaskGetTickCount();
             
            camera_data.image_data->resize(
                camera_data.width * camera_data.height * 
                CameraFormatBpp(camera_data.format));

            CameraFrameFormat fmt{
                camera_data.format,
                CameraConfig::filter,
                CameraConfig::rotation,
                static_cast<int>(camera_data.width),
                static_cast<int>(camera_data.height),
                false,
                camera_data.image_data->data(),
                CameraConfig::auto_white_balance
            };

            if (CameraTask::GetSingleton()->GetFrame({fmt})) {
                // Now safe to send to queue
                if (xQueueOverwrite(g_camera_queue_m4, &camera_data) != pdTRUE) {
                    printf("Failed to send camera data to queue\r\n");
                }
            } else {
                printf("Camera frame capture failed\r\n");
            }

            vTaskDelay(pdMS_TO_TICKS(33));  // ~30 FPS
        }
    }
}