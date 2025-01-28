// rpc_task.cc
#include "m7/rpc_task.hh"

namespace coralmicro {

static void print_start_message() {
    MulticoreMutexLock lock(0);
    printf("M7 RPC task starting...\r\n");
}

static void print_stream_service_info(std::string usb_ip) {
    MulticoreMutexLock lock(0);
    printf("Starting Stream Service on: %s\r\n", usb_ip.c_str());
}

bool read_first_frame = false;

void get_frame(struct jsonrpc_request* request) {
    auto& shared = SharedMemory::GetInstance();
    
    MulticoreMutexLock lock(kSharedMemoryMutex);
    
    if (!shared.new_frame_ready) {
        jsonrpc_return_error(request, -1, "No camera data available", nullptr);
        return;
    }

    // Send the frame
    jsonrpc_return_success(
        request,
        "{%Q: %d, %Q: %d, %Q: %V}",
        "width", shared.process_frame.width,
        "height", shared.process_frame.height,
        "base64_data", shared.process_frame.data_size, 
        shared.process_frame.data
    );

    // Mark frame as consumed
    shared.new_frame_ready = false;

    if (!read_first_frame) {
        MulticoreMutexLock lock(0);
        printf("First frame sent to host\r\n");
        read_first_frame = true;
    }
}

bool first_tof_frame = false;

void get_tof_grid(struct jsonrpc_request* request) {
    // TOF handling remains unchanged
    VL53L8CX_ResultsData results;
    
    if (xQueuePeek(g_tof_queue_m7, &results, 0) != pdTRUE) {
        jsonrpc_return_error(request, -1, "No TOF data available", nullptr);
        return;
    }

    std::string distances_array = "[";
    for (int i = 0; i < 64; i++) {
        if (results.nb_target_detected[i] > 0) {
            distances_array += std::to_string(results.distance_mm[i]);
        } else {
            distances_array += "0";
        }
        if (i < 63) distances_array += ",";
    }
    distances_array += "]";

    jsonrpc_return_success(
        request,
        "{%Q: %s, %Q: %d}",
        "distances", distances_array.c_str(),
        "temperature", results.silicon_temp_degc
    );

    if (!first_tof_frame) {
        MulticoreMutexLock lock(0);
        printf("First TOF frame sent to host\r\n");
        first_tof_frame = true;
    }
}

void rpc_task(void* parameters) {
    (void)parameters;
    print_start_message();
    
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    std::string usb_ip;
    if (!GetUsbIpAddress(&usb_ip)) {
        MulticoreMutexLock lock(0);
        printf("Failed to get USB IP Address\r\n");
        vTaskSuspend(nullptr);
    }

    print_stream_service_info(usb_ip);

    jsonrpc_init(nullptr, nullptr);
    jsonrpc_export("get_image_from_camera", get_frame);
    jsonrpc_export("get_tof_grid", get_tof_grid);

    auto* server = new JsonRpcHttpServer();
    if (!server) {
        MulticoreMutexLock lock(0);
        printf("Failed to create RPC HTTP server\r\n");
        vTaskSuspend(nullptr);
    }

    UseHttpServer(server);

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

}