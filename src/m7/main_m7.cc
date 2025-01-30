// main_m7.cc
#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/task.h"

// Starting M4 core 
#include "startup_banner.hh"

#include "m7/task_config_m7.hh"
#include "m7/m7_queues.hh"

#include <cstdio>
#include "libs/base/led.h"

namespace coralmicro {
namespace {

    void setup_tasks() {
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

        // Initialize M7 tasks
        setup_tasks();

        // Set status LED
        LedSet(Led::kStatus, true);


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