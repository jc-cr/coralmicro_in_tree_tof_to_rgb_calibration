// rpc_task.cc
#include "rpc_task.hh"

namespace coralmicro {

// RPC handler that returns latest data from both sensors
void get_synchronized_frame(struct jsonrpc_request* request) {
    CameraData camera_data;
    VL53L8CX_ResultsData tof_data;
    
    // Try to get latest TOF data without waiting (peek)
    if (xQueuePeek(g_tof_queue, &tof_data, 0) != pdTRUE) {
        jsonrpc_return_error(request, -1, "No TOF data available", nullptr);
        return;
    }

    // Try to get latest camera frame without waiting (peek)
    if (xQueuePeek(g_camera_queue, &camera_data, 0) != pdTRUE) {
        jsonrpc_return_error(request, -1, "No camera data available", nullptr);
        return;
    }

    // Calculate frame time difference using camera timestamp only
    // Note: TOF data doesn't provide timestamps, so we can only use camera timestamps
    TickType_t current_time = xTaskGetTickCount();
    TickType_t time_since_camera = current_time - camera_data.timestamp;

    // Return synchronized data
    jsonrpc_return_success(
        request,
        "{"
        "\"width\": %u,"
        "\"height\": %u,"
        "\"rgb_data\": %V,"
        "\"tof_data\": %V,"
        "\"time_since_capture_ms\": %d,"
        "\"tof_temp\": %d"
        "}",
        camera_data.width,
        camera_data.height,
        camera_data.image_data.size(), camera_data.image_data.data(),
        sizeof(tof_data.distance_mm), tof_data.distance_mm,
        static_cast<int>(time_since_camera * portTICK_PERIOD_MS),
        tof_data.silicon_temp_degc
    );
}

void rpc_task(void* parameters) {
    (void)parameters;
    
    printf("RPC task starting...\r\n");
    
    // Initialize USB communication
    std::string usb_ip;
    if (!GetUsbIpAddress(&usb_ip)) {
        printf("Failed to get USB IP Address\r\n");
        vTaskSuspend(nullptr);
    }
    printf("Starting Stream Service on: %s\r\n", usb_ip.c_str());

    // Setup RPC endpoint  
    jsonrpc_init(nullptr, nullptr);
    jsonrpc_export("get_synchronized_frame", get_synchronized_frame);
    UseHttpServer(new JsonRpcHttpServer);

    vTaskSuspend(nullptr);
}

} // namespace coralmicro