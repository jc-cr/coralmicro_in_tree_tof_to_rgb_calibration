// camera_task.cc
#include "camera_task.hh"

namespace coralmicro {

void camera_task(void* parameters) {
  (void)parameters;

  printf("Camera task starting...\r\n");

  // Initialize camera
  CameraTask::GetSingleton()->SetPower(true);
  CameraTask::GetSingleton()->Enable(CameraMode::kStreaming);

  // Main task loop
  TickType_t last_wake_time = xTaskGetTickCount();
  const TickType_t frequency = pdMS_TO_TICKS(33); // ~30fps
  
  while (true) {
    // Capture image
    CameraData camera_data;
    camera_data.width = 324;  // Native resolution
    camera_data.height = 324;
    camera_data.format = CameraFormat::kRgb;
    camera_data.timestamp = xTaskGetTickCount();
    
    camera_data.image_data.resize(
        camera_data.width * camera_data.height * 
        CameraFormatBpp(camera_data.format));

    CameraFrameFormat fmt{
        camera_data.format,
        CameraFilterMethod::kBilinear,
        CameraRotation::k270,
        static_cast<int>(camera_data.width),
        static_cast<int>(camera_data.height),
        false,
        camera_data.image_data.data()
    };

    if (CameraTask::GetSingleton()->GetFrame({fmt})) {
      // Send latest frame to queue
      xQueueOverwrite(g_camera_queue, &camera_data);
    } else {
      printf("Camera frame capture failed\r\n");
    }

    // Use vTaskDelayUntil for consistent timing
    vTaskDelayUntil(&last_wake_time, frequency);
  }
}

} // namespace coralmicro