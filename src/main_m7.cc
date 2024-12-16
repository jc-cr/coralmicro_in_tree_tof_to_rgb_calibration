#include "task_config_m7.hh"
#include "logo.hh"

#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/task.h"

namespace coralmicro {
namespace {

const char* PROJECT_NAME = "PCB Bringup";

TaskErr_t setup_tasks() {
    printf("Starting M7 task creation...\r\n");
    TaskErr_t ret = CreateM7Tasks();
    printf("M7 task creation returned: %d\r\n", static_cast<int>(ret));
    return ret;
}

[[noreturn]] void main_m7() {
    // Print startup banner
    printf("\n%s\r\n", PROJECT_NAME);
    printf("Developed by JC \r\n");
    printf("%s\r\n\n", PROJECT_LOGO);

    // Initialize M7 tasks
    TaskErr_t task_status = setup_tasks();
    if (task_status != TaskErr_t::OK) {
        printf("Failed to initialize M7 tasks, suspending...\r\n");
        vTaskSuspend(nullptr);
    }

    printf("Entering M7 main loop\r\n");
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
    vTaskSuspend(nullptr);
}