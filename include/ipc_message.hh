// ipc_message.hh
#pragma once
#include "libs/base/ipc_message_buffer.h"

namespace coralmicro {

    // Global IPC message handler for use in M4 core initialization by M7 core
    inline std::function<void(const uint8_t data[kIpcMessageBufferDataSize])> g_ipc_task_handler;

    // Simple notification message type
    enum class AppMessageType : uint8_t {
        kNewFrame,    // Signals new frame is ready in shared memory
    };

    struct NotificationMessage {
        AppMessageType type;  // Must be kNewFrame
    } __attribute__((packed));


    // Global IPC message handler
    extern void rx_data(const uint8_t data[kIpcMessageBufferDataSize]);

} // namespace coralmicro