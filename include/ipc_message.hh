// ipc_message.hh
#pragma once
#include "libs/base/ipc_message_buffer.h"
#include "libs/camera/camera.h"

namespace coralmicro {

    // Global IPC message handler for use in M4 core initialization by M7 core
    inline std::function<void(const uint8_t data[kIpcMessageBufferDataSize])> g_ipc_task_handler;


    // Define message types first
    enum class AppMessageType : uint8_t {
        kCameraData,
        kStateEvent,
    };

    static_assert(sizeof(AppMessageType) == 1, "AppMessageType must be 1 byte");

    struct CameraDataIPC {
        uint32_t width;
        uint32_t height;
        CameraFormat format;
        TickType_t timestamp;
        uint32_t data_size;
        uint8_t data[kIpcMessageBufferDataSize - sizeof(AppMessageType) - 
                    sizeof(uint32_t) * 4 - sizeof(CameraFormat) - sizeof(TickType_t)];
    } __attribute__((packed));


    struct CameraDataMessage {
        AppMessageType type;  // Must be kCameraData
        uint32_t width;
        uint32_t height;
        CameraFormat format;
        TickType_t timestamp;
        uint32_t data_size;
        uint8_t data[kIpcMessageBufferDataSize - sizeof(AppMessageType) - 
                    sizeof(uint32_t) * 4 - sizeof(CameraFormat) - sizeof(TickType_t)];
    } __attribute__((packed));

    static_assert(sizeof(CameraDataMessage) <= kIpcMessageBufferDataSize,
                 "CameraDataMessage exceeds IPC buffer size");

    // Global IPC message handlers
    extern void rx_data(const uint8_t data[kIpcMessageBufferDataSize]);
    extern void tx_data();

} // namespace coralmicro