cmake_minimum_required(VERSION 3.22.0)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(ventrilation VERSION 0.1.0 LANGUAGES C CXX ASM)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# variables to subdirectories
set(modbus_src ${CMAKE_CURRENT_LIST_DIR}/src/modbus_handler)
set(pressure_src ${CMAKE_CURRENT_LIST_DIR}/src/pressure_handler)
set (pico_hw_src ${CMAKE_CURRENT_LIST_DIR}/src/pico_hw)
set (fan_src ${CMAKE_CURRENT_LIST_DIR}/src/fan_controller)

# includes
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src/modbus_handler)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src/pressure_handler)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src/fan_controller)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src/pico_hw)


add_compile_options(-Wall)

add_executable(${PROJECT_NAME} main.cpp)

# HARDWARE classes
add_library(uart_instance ${pico_hw_src}/uart_instance.cpp)
add_library(i2c_instance ${pico_hw_src}/i2c_instance.cpp)
add_library(dma_channel ${pico_hw_src}/dma_channel.cpp)
# MODBUS handler
add_library(modbusreg ${modbus_src}/modbus_register.cpp)
add_library(modbusctrl ${modbus_src}/modbus_controller.cpp)
# PRESSURE
add_library(pressure ${pressure_src}/pressure.cpp)
# FAN
add_library(fan ${fan_src}/fan.cpp)


pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} pico_stdlib modbusreg pressure fan hardware_i2c)

# HARDWARE class links
target_link_libraries(uart_instance     pico_stdlib hardware_uart)
target_link_libraries(i2c_instance      pico_stdlib hardware_i2c)
target_link_libraries(dma_channel       pico_stdlib hardware_dma)

# MODBUS handler links
target_link_libraries(modbusreg         pico_stdlib modbusctrl)
target_link_libraries(modbusctrl        pico_stdlib uart_instance dma_channel)

# PRESSURE
target_link_libraries(pressure pico_stdlib i2c_instance)

#FAN
target_link_libraries(fan      pico_stdlib pressure modbusreg)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
