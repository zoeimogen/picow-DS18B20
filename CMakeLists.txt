cmake_minimum_required(VERSION 3.13)
include( $ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake )

project(picow_DS18B20 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)

add_subdirectory(modules/pico-onewire)

include_directories(.)
pico_sdk_init()

add_executable(picow_DS18B20
  picow_DS18B20.cpp
)

pico_enable_stdio_usb(picow_DS18B20 1)  
pico_enable_stdio_uart(picow_DS18B20 0)
pico_add_extra_outputs(picow_DS18B20)

target_link_libraries(picow_DS18B20
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mqtt
    hardware_adc
    pico_one_wire
)