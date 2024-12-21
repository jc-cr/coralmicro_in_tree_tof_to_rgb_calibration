// rpc_task.cc
#include "rpc_task.hh"

namespace coralmicro {

    void get_frame(struct jsonrpc_request* request) {
        CameraData camera_data;
        
        // Try to get latest camera frame without waiting
        if (xQueuePeek(g_camera_queue, &camera_data, 0) != pdTRUE) {
            jsonrpc_return_error(request, -1, "No camera data available", nullptr);
            return;
        }

        // Return the frame data in the same format as the example
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
        jsonrpc_export("get_image_from_camera", get_frame);
        UseHttpServer(new JsonRpcHttpServer);

        vTaskSuspend(nullptr);
    }
}