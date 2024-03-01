cmake_minimum_required(VERSION 3.22.0)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(ventrilation VERSION 0.1.0 LANGUAGES C CXX ASM)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(modbus_src ${CMAKE_CURRENT_LIST_DIR}/src/modbus_handler)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src/modbus_handler)

add_compile_options(-Wall)

add_executable(${PROJECT_NAME} main.cpp)
add_library(pin src/pin.cpp)
add_library(uart_instance ${modbus_src}/uart_instance.cpp)
add_library(dma_buffer ${modbus_src}/dma_buffer.cpp)
add_library(modbus ${modbus_src}/modbus.cpp)


pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} pico_stdlib pin modbus)
target_link_libraries(pin         pico_stdlib)
target_link_libraries(uart_instance pico_stdlib hardware_uart)
target_link_libraries(dma_buffer uart_instance pico_stdlib hardware_uart hardware_dma hardware_timer)
target_link_libraries(modbus dma_buffer pico_stdlib hardware_uart hardware_dma)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)


