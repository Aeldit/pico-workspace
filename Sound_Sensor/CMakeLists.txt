add_executable(Sound_Sensor
Sound_Sensor.cpp
        )

# pull in common dependencies
target_link_libraries(Sound_Sensor pico_stdlib pico_stdio hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(Sound_Sensor)

# Enable usb output, disable uart output
pico_enable_stdio_usb(Sound_Sensor 1)
pico_enable_stdio_uart(Sound_Sensor 0)
