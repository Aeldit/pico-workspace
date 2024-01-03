add_executable(sound_sensor
        sound_sensor.cpp
)

target_sources(sound_sensor
        PUBLIC
                ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/SSD1306-Oled-Pico/SSD1306.cpp
                ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/SSD1306-Oled-Pico/GFX.cpp
                ${CMAKE_CURRENT_LIST_DIR}/screens.cpp
)


target_include_directories(sound_sensor
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/SSD1306-Oled-Pico/
)

# pull in common dependencies
target_link_libraries(sound_sensor pico_stdlib pico_stdio hardware_adc hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(sound_sensor)

# Enable usb output, disable uart output
pico_enable_stdio_usb(sound_sensor 1)
pico_enable_stdio_uart(sound_sensor 0)
