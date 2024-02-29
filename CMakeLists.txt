cmake_minimum_required(VERSION 3.22.0)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(PICO_BOARD pico_w)

project(ventrilation VERSION 0.1.0 LANGUAGES C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(source_location ${CMAKE_CURRENT_LIST_DIR}/src)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/paho.mqtt.embedded-c/MQTTPacket/src
        ${CMAKE_CURRENT_LIST_DIR}/paho.mqtt.embedded-c/MQTTClient/src
)

add_compile_options(-Wall)

file(GLOB MQTT_SOURCES "paho.mqtt.embedded-c/MQTTPacket/src/*.c")

add_executable(${PROJECT_NAME} main.cpp
        src/IPStack.cpp
        src/IPStack.h
        src/Countdown.cpp
        src/Countdown.h
        src/commhandler.cpp
        src/commhandler.h
        ${MQTT_SOURCES}
)
add_library(pin src/pin.cpp)

pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} pico_stdlib pin pico_cyw43_arch_lwip_poll)
target_link_libraries(pin         pico_stdlib)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)


