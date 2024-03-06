cmake_minimum_required(VERSION 3.22.0)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(ventrilation VERSION 0.1.0 LANGUAGES C CXX ASM)

pico_sdk_init()

set(source_location ${CMAKE_CURRENT_LIST_DIR}/src)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

add_compile_options(-Wall)

add_executable(${PROJECT_NAME} main.cpp
        src/display/font_petme128_8x8.h
        src/display/ssd1306.h
        src/display/mono_vlsb.h
        src/display/framebuf.h
        src/display/mono_vlsb.cpp
        src/display/framebuf.cpp
        src/display/ssd1306.cpp
        src/display/graphics.cpp
        src/display/graphics.h
        src/input.cpp)
add_library(pin src/pin.cpp
        )


pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} pico_stdlib pin)
target_link_libraries(pin         pico_stdlib)
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_i2c)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)


