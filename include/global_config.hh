// global_config.hh
#pragma once

#include <atomic>

extern "C" {
#include "vl53l8cx_api.h"
}

namespace coralmicro {
    // Global configuration values
    inline std::atomic<uint8_t> g_tof_resolution{VL53L8CX_RESOLUTION_4X4};  // Default to 4x4
}