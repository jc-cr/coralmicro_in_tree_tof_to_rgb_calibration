// rpc_task.hh

#include "libs/rpc/rpc_http_server.h"
#include "libs/rpc/rpc_utils.h"
#include "libs/base/utils.h"
#include "third_party/mjson/src/mjson.h"

#include "libs/base/led.h"

#include "sensor_queues.hh"

namespace coralmicro {
    void rpc_task(void* parameters);
    void get_synchronized_frame(struct jsonrpc_request* request);
}