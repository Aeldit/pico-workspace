add_executable(SSD1306_example
SSD1306_example.cpp
)

# Add sources
target_sources(SSD1306_example
        PUBLIC
                ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/SSD1306-Oled-Pico/SSD1306.cpp
                ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/SSD1306-Oled-Pico/GFX.cpp
)

# Add headers
target_include_directories(SSD1306_example
        PUBLIC
                ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/SSD1306-Oled-Pico/
)

# Add dependencies
target_link_libraries(SSD1306_example pico_stdlib pico_stdio hardware_i2c)


# create map/bin/hex file etc.
pico_add_extra_outputs(SSD1306_example)


# Enable usb output, disable uart output
pico_enable_stdio_usb(SSD1306_example 1)
