// main_m7.cc
#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/task.h"

// Starting M4 core 
#include "libs/base/ipc_m7.h"
#include "libs/base/mutex.h"

#include "startup_banner.hh"

#include "m7/task_config_m7.hh"
#include "m7/m7_queues.hh"
#include "ipc_message.hh"


namespace coralmicro {
namespace {

    void start_m4() {
        auto* ipc = IpcM7::GetSingleton();
        
        // Set up IPC handler first
        g_ipc_task_handler = rx_data;  // Moved from M7 IPC task
        
        // Then register global message handler
        ipc->RegisterAppMessageHandler([](const uint8_t data[kIpcMessageBufferDataSize]) {
            if (g_ipc_task_handler) {
                g_ipc_task_handler(data);
            }
        });

        // Start M4 core only after everything is set up
        ipc->StartM4();
        CHECK(ipc->M4IsAlive(500));

        MulticoreMutexLock lock(0);
        printf("M4 core started successfully\r\n");
    }

    void setup_tasks() {
        MulticoreMutexLock lock(0);
        printf("Starting M7 task creation...\r\n");
        
        // Task creation - queues already initialized in start_m4()
        if (CreateM7Tasks() != TaskErr_t::OK)
        {
            printf("Failed to create M7 tasks\r\n");
            vTaskSuspend(nullptr);
        }
    }

    [[noreturn]] void main_m7() {
        // Print startup banner
        print_startup_banner();

        // Initialize queues first
        if (!InitQueues()) {
            printf("Failed to initialize queues\r\n");
            vTaskSuspend(nullptr);
        }

        // Start M4 core (and initialize queues)
        start_m4();

        // Initialize M7 tasks
        setup_tasks();

        while (true) {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    } 

} // namespace
} // namespace coralmicro

extern "C" void app_main(void* param) {
    (void)param;
    printf("Starting M7 initialization...\r\n");
    coralmicro::main_m7();

    // Should never reach here as main_m7 is a loop
    vTaskSuspend(nullptr);
}