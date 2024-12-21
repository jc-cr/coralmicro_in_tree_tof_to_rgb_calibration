#include "camera_task.hh"

namespace coralmicro {

void camera_task(void* parameters) {
    (void)parameters;

    printf("Camera task starting...\r\n");

    // Initialize camera
    CameraTask::GetSingleton()->SetPower(true);
    CameraTask::GetSingleton()->Enable(CameraMode::kStreaming);

    // Main task loop
    
    while (true) {
        // Create new camera data instance
        CameraData camera_data;
        camera_data.width = 324;
        camera_data.height = 324;
        camera_data.format = CameraFormat::kRaw;
        camera_data.timestamp = xTaskGetTickCount();
        
        // Resize the vector in the shared pointer
        camera_data.image_data->resize(
            camera_data.width * camera_data.height * 
            CameraFormatBpp(camera_data.format));

        CameraFrameFormat fmt{
            camera_data.format,
            CameraFilterMethod::kBilinear,
            CameraRotation::k270,
            static_cast<int>(camera_data.width),
            static_cast<int>(camera_data.height),
            false,
            camera_data.image_data->data(),
            false
        };

        if (CameraTask::GetSingleton()->GetFrame({fmt})) {
            // Now safe to send to queue since we're using shared_ptr
            if (xQueueOverwrite(g_camera_queue, &camera_data) != pdTRUE) {
                printf("Failed to send camera data to queue\r\n");
            }
        } else {
            printf("Camera frame capture failed\r\n");
        }

        // 15 fps (33ms) delay
        vTaskDelay(pdMS_TO_TICKS(33));
    }
}

} // namespace coralmicro