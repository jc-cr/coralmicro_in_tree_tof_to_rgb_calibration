cmake_minimum_required(VERSION 3.16)
project(coralmicro_in_tree_tof_to_rgb_calibration)

add_subdirectory(libs/coralmicro_VL53L8CX_ULD_driver)


# Define task source files for each core
set(M4_TASK_SOURCES
)

set(M7_TASK_SOURCES
    src/tof_task.cc
    src/camera_task.cc
    src/rpc_task.cc
)

# Define paths for task configuration
set(TASK_CONFIG_YAML "${CMAKE_CURRENT_SOURCE_DIR}/freertos_task_config_generator/tasks_config.yaml")
set(TASK_CONFIG_M7_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/task_config_m7.hh")
set(TASK_CONFIG_M7_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/task_config_m7.cc")
set(TASK_CONFIG_M4_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/task_config_m4.hh")
set(TASK_CONFIG_M4_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/task_config_m4.cc")
set(TASK_GENERATOR_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/freertos_task_config_generator/generate_tasks.py")

# Add custom command to generate task configuration
add_custom_command(
    OUTPUT 
        ${TASK_CONFIG_M7_HEADER}
        ${TASK_CONFIG_M7_SOURCE}
        ${TASK_CONFIG_M4_HEADER}
        ${TASK_CONFIG_M4_SOURCE}
    COMMAND python3 ${TASK_GENERATOR_SCRIPT} 
            ${TASK_CONFIG_YAML}
            ${TASK_CONFIG_M7_HEADER} 
            ${TASK_CONFIG_M7_SOURCE}
            ${TASK_CONFIG_M4_HEADER}
            ${TASK_CONFIG_M4_SOURCE}
    DEPENDS ${TASK_CONFIG_YAML} ${TASK_GENERATOR_SCRIPT}
    COMMENT "Generating task configuration files"
    VERBATIM
)

# Create custom targets for each core
add_custom_target(${PROJECT_NAME}_generate_task_config
    DEPENDS 
        ${TASK_CONFIG_M7_HEADER}
        ${TASK_CONFIG_M7_SOURCE}
        ${TASK_CONFIG_M4_HEADER}
        ${TASK_CONFIG_M4_SOURCE}
)


# M7 Core Executable
add_executable_m7(${PROJECT_NAME}
    src/main_m7.cc
    ${TASK_CONFIG_M7_SOURCE}
    ${M7_TASK_SOURCES}
)

# Set include directories for M7
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/coralmicro_VL53L8CX_ULD_driver/Platform
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/coralmicro_VL53L8CX_ULD_driver/VL53L8CX_ULD_API/inc
)

# Add dependency on task configuration for M7
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_generate_task_config)

# Link libraries for M7
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        libs_rpc_http_server
        libs_rpc_utils
        vl53l8cx_driver
        libs_base-m7_freertos
)

# M7 compiler flags
target_compile_options(${PROJECT_NAME}
    PRIVATE
        -mcpu=cortex-m7
        -mthumb
        -mfpu=fpv5-d16
        -mfloat-abi=hard
        -Os
        -ffunction-sections
        -fdata-sections
        -fno-exceptions
        -fno-rtti
        -g0
        -ffast-math
        -fshort-enums
        -fno-unwind-tables
        -fno-asynchronous-unwind-tables
        -Wall
        -Wextra
        $<$<COMPILE_LANGUAGE:C>:-std=c11>
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
)

# M7 linker flags
target_link_options(${PROJECT_NAME}
    PRIVATE
        -mcpu=cortex-m7
        -mthumb
        -mfpu=fpv5-d16
        -mfloat-abi=hard
        -Wl,-Map=${PROJECT_NAME}.map
        -Wl,--print-memory-usage
        -Wl,--gc-sections
        -Wl,--sort-section=alignment
        -Wl,--cref
)