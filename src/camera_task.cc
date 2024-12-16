#include "camera_task.hh"

namespace coralmicro {
namespace {



    // Initialize Camera
    CameraTask::GetSingleton()->SetPower(true);
    CameraTask::GetSingleton()->Enable(CameraMode::kStreaming);

    // Structure to hold TOF data in the JSON response
    struct TofData {
    uint16_t distance_mm[64];  // 8x8 grid
    uint8_t target_status[64]; // Target validity status
    int8_t temperature;        // Silicon temperature
    };

    void GetSynchronizedFrame(struct jsonrpc_request* request) {
    int width;
    if (!JsonRpcGetIntegerParam(request, "width", &width)) return;

    int height;
    if (!JsonRpcGetIntegerParam(request, "height", &height)) return;

    std::string format_rpc;
    if (!JsonRpcGetStringParam(request, "format", &format_rpc)) return;

    auto format = CheckCameraFormat(format_rpc);
    if (!format.has_value()) {
        jsonrpc_return_error(request, -1, "Unknown 'format'", nullptr);
        return;
    }

    // Get RGB frame
    std::vector<uint8_t> image(width * height * CameraFormatBpp(*format));
    CameraFrameFormat fmt{
        *format,
        CameraFilterMethod::kBilinear,
        CameraRotation::k270,
        width,
        height,
        false,
        image.data()
    };

    if (!CameraTask::GetSingleton()->GetFrame({fmt})) {
        jsonrpc_return_error(request, -1, "Failed to get RGB frame", nullptr);
        return;
    }

    // Get TOF data
    TofData tof_data;
    VL53L8CX_ResultsData results;
    
    uint8_t tof_ready = 0;
    if (vl53l8cx_check_data_ready(&tof_dev, &tof_ready) != VL53L8CX_STATUS_OK || !tof_ready) {
        jsonrpc_return_error(request, -1, "Failed to get TOF data", nullptr);
        return;
    }

    if (vl53l8cx_get_ranging_data(&tof_dev, &results) != VL53L8CX_STATUS_OK) {
        jsonrpc_return_error(request, -1, "Failed to read TOF data", nullptr);
        return;
    }

    // Copy TOF data into our structure
    memcpy(tof_data.distance_mm, results.distance_mm, sizeof(tof_data.distance_mm));
    memcpy(tof_data.target_status, results.target_status, sizeof(tof_data.target_status));
    tof_data.temperature = results.silicon_temp_degc;

    // Return synchronized data
    jsonrpc_return_success(
        request,
        "{"
        "%Q: %d,"    // width
        "%Q: %d,"    // height 
        "%Q: %V,"    // RGB data
        "%Q: %V,"    // TOF distances
        "%Q: %V,"    // TOF target status
        "%Q: %d"     // TOF temperature
        "}",
        "width", width,
        "height", height,
        "rgb_data", image.size(), image.data(),
        "tof_distances", sizeof(tof_data.distance_mm), tof_data.distance_mm,
        "tof_status", sizeof(tof_data.target_status), tof_data.target_status,
        "tof_temp", tof_data.temperature
    );
    }




    
}

} // namespace
