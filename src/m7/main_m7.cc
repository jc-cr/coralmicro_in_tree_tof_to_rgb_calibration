// main_m7.cc
#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/task.h"

// Starting M4 core 
#include "libs/base/ipc_m7.h"
#include "libs/base/mutex.h"

#include "startup_banner.hh"

#include "m7/task_config_m7.hh"
#include "m7/m7_queues.hh"



namespace coralmicro {
namespace {

void setup_tasks() {
    MulticoreMutexLock lock(0);
    printf("Starting M7 task creation...\r\n");

    // Init queues
    // BOOL return type
    if (!InitQueues()) {
        printf("Failed to initialize queues\r\n");
        vTaskSuspend(nullptr);
    }
    
    // Task creation
    // TaskErr_t return type
    if (CreateM7Tasks() != TaskErr_t::OK)
    {
        printf("Failed to create M7 tasks\r\n");
        vTaskSuspend(nullptr);
    }
}

void start_m4() {
    // Start M4 core
    coralmicro::IpcM7::GetSingleton()->StartM4();
    CHECK(coralmicro::IpcM7::GetSingleton()->M4IsAlive(500));

    MulticoreMutexLock lock(0);
    printf("M4 core started successfully\r\n");
}

[[noreturn]] void main_m7() {
    // Print startup banner
    print_startup_banner();

    // Start M4 core
    start_m4();

    // Now any prints need mutex

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