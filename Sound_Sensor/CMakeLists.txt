add_executable(sound_sensor
        sound_sensor.cpp
)

# pull in common dependencies
target_link_libraries(sound_sensor pico_stdlib pico_stdio hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(sound_sensor)

# Enable usb output, disable uart output
pico_enable_stdio_usb(sound_sensor 1)
pico_enable_stdio_uart(sound_sensor 0)
