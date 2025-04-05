# touch_capacitive/CMakeLists.txt

add_library(touch_capacitive
    touch_capacitive.c
)

target_include_directories(touch_capacitive PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(touch_capacitive
    hardware_i2c
    hardware_gpio
    pico_stdlib
)
