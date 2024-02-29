cmake_minimum_required(VERSION 3.22.0)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(ventrilation VERSION 0.1.0 LANGUAGES C CXX ASM)

pico_sdk_init()

set(source_location ${CMAKE_CURRENT_LIST_DIR}/src)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

add_compile_options(-Wall)

add_executable(${PROJECT_NAME} main.cpp)
add_library(pin src/pin.cpp)
add_library(eeprom src/eeprom.c)
add_library(logHandler src/logHandler.cpp)

pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} pico_stdlib pin hardware_i2c logHandler eeprom)
target_link_libraries(pin         pico_stdlib)
target_link_libraries(eeprom      pico_stdlib hardware_i2c)
target_link_libraries(logHandler  pico_stdlib eeprom)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
