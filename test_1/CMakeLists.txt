add_executable(test_1
test_1.cpp
        )

target_sources(test_1
        PUBLIC
                ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/SSD1306-Oled-Pico/SSD1306.cpp
                ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/SSD1306-Oled-Pico/GFX.cpp
                ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/dht22_pico/dht22_pico.cpp
                ${CMAKE_CURRENT_LIST_DIR}/screens.cpp
)


target_include_directories(test_1
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/SSD1306-Oled-Pico/
            ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/dht22_pico/
)

# pull in common dependencies
target_link_libraries(test_1 pico_stdlib pico_stdio hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(test_1)

# add url via pico_set_program_url
#example_auto_set_url(LED_LCD_i2c)

# Enable usb output, disable uart output
pico_enable_stdio_usb(test_1 1)
pico_enable_stdio_uart(test_1 0)
