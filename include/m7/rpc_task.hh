// rpc_task.hh

#include "libs/rpc/rpc_http_server.h"
#include "libs/rpc/rpc_utils.h"
#include "libs/base/utils.h"
#include "third_party/mjson/src/mjson.h"

#include "libs/base/led.h"

#include "m7/m7_queues.hh"

namespace coralmicro {
    
    struct RpcTaskQueues {
        static constexpr QueueHandle_t* camera_queue = &g_ipc_camera_queue_m7;
        static constexpr QueueHandle_t* tof_queue = &g_tof_queue_m7;
    };

    void rpc_task(void* parameters);
    void get_frame(struct jsonrpc_request* request);
    void get_tof_grid(struct jsonrpc_request* request);
}