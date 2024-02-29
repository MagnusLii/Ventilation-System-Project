cmake_minimum_required(VERSION 3.22.0)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(ventrilation VERSION 0.1.0 LANGUAGES C CXX ASM)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(source_location ${CMAKE_CURRENT_LIST_DIR}/src)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

add_compile_options(-Wall)

add_executable(${PROJECT_NAME} main.cpp)
add_library(pin src/pin.cpp)
add_library(modbus src/modbus.cpp)


pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} pico_stdlib pin modbus)
target_link_libraries(pin         pico_stdlib)
target_link_libraries(modbus pico_stdlib hardware_uart hardware_dma)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)


