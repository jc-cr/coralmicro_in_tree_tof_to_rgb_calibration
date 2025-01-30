// camera_task.cc
#include "m7/camera_task.hh"

namespace coralmicro {

bool first_frame_captured_flag = false;

void camera_task(void* parameters) {
    (void)parameters;
    printf("Camera task starting...\r\n");

    // Initialize camera with proper error checking
    if (!CameraTask::GetSingleton()->SetPower(true)) {
        printf("Failed to power on camera\r\n");
        vTaskSuspend(nullptr);
        return;
    }

    if (!CameraTask::GetSingleton()->Enable(CameraMode::kStreaming)) {
        printf("Failed to enable camera streaming\r\n");
        vTaskSuspend(nullptr);
        return;
    }

    // Discard initial frames to allow auto-exposure calibration
    CameraTask::GetSingleton()->DiscardFrames(100);


  // Initialize two camera data buffers
    CameraData camera_data;
    camera_data.width = CameraConfig::kWidth;
    camera_data.height = CameraConfig::kHeight;
    camera_data.format = CameraConfig::kFormat;

    // Pre-allocate buffer
    const size_t buffer_size = camera_data.width * camera_data.height * 
                              CameraFormatBpp(camera_data.format);
    camera_data.image_data.reserve(buffer_size);
    camera_data.image_data.resize(buffer_size);

    // Create frame format structure
    CameraFrameFormat fmt{
        camera_data.format,
        CameraConfig::filter,
        CameraConfig::rotation,
        static_cast<int>(camera_data.width),
        static_cast<int>(camera_data.height),
        CameraConfig::preserve_ratio,
        camera_data.image_data.data(),  // Direct access to vector data
        CameraConfig::auto_white_balance
    };

 // Track timing for consistent frame rate
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t capture_period = pdMS_TO_TICKS(33);  // ~30 fps

    while (true) {
        if (CameraTask::GetSingleton()->GetFrame({fmt})) {
            camera_data.timestamp = xTaskGetTickCount();
            
            if (xQueueOverwrite(g_camera_queue_m7, &camera_data) != pdTRUE) {
                printf("Failed to send camera data to queue\r\n");
            }
        }
        
        // Use vTaskDelayUntil for consistent frame timing
        vTaskDelayUntil(&last_wake_time, capture_period);
    }
}


} // namespace coralmicro