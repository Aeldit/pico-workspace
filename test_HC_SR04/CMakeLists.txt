add_executable(test_HC_SR04
test_HC_SR04.cpp
        )

# pull in common dependencies
target_link_libraries(test_HC_SR04 pico_stdlib pico_stdio hardware_sync)

# create map/bin/hex file etc.
pico_add_extra_outputs(test_HC_SR04)

# Enable usb output, disable uart output
pico_enable_stdio_usb(test_HC_SR04 1)
pico_enable_stdio_uart(test_HC_SR04 0)
