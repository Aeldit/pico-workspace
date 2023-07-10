add_executable(SSD1306
SSD1306_example.cpp
)
# Add headers
target_include_directories(SSD1306 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/
)

# Add sources
target_sources(SSD1306 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/SSD1306.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GFX.cpp
)

# Add dependencies
target_link_libraries(SSD1306 pico_stdlib hardware_i2c)


# create map/bin/hex file etc.
pico_add_extra_outputs(SSD1306)


# Enable usb output, disable uart output
pico_enable_stdio_usb(SSD1306 1)