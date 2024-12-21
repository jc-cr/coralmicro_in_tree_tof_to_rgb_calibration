// rpc_task.cc
#include "rpc_task.hh"

namespace coralmicro {

void get_synchronized_frame(struct jsonrpc_request* request) {
    CameraData camera_data;
    
    // Try to get latest camera frame without waiting
    if (xQueuePeek(g_camera_queue, &camera_data, 0) != pdTRUE) {
        jsonrpc_return_error(request, -1, "No camera data available", nullptr);
        return;
    }

    // Calculate time since capture
    TickType_t current_time = xTaskGetTickCount();
    TickType_t time_since_capture = current_time - camera_data.timestamp;

    // Return the frame data
    jsonrpc_return_success(
        request,
        "{%Q: %d, %Q: %d, %Q: %V}",
        "width", camera_data.width,
        "height", camera_data.height,
        "base64_data", camera_data.image_data->size(), camera_data.image_data->data()
    );
}

void rpc_task(void* parameters) {
    (void)parameters;
    
    printf("RPC task starting...\r\n");
    
    std::string usb_ip;
    if (!GetUsbIpAddress(&usb_ip)) {
        printf("Failed to get USB IP Address\r\n");
        vTaskSuspend(nullptr);
    }
    printf("Starting Stream Service on: %s\r\n", usb_ip.c_str());

    jsonrpc_init(nullptr, nullptr);
    jsonrpc_export("get_synchronized_frame", get_synchronized_frame);
    UseHttpServer(new JsonRpcHttpServer);

    vTaskSuspend(nullptr);
}

} // namespace coralmicro