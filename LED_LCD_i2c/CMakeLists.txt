add_executable(LED_LCD_i2c
LED_LCD_i2c.cpp
        )

target_sources(LED_LCD_i2c
        PUBLIC
                ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/dht22_Pico/dht22_pico.cpp
                ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/RPi-Pico-SSD1306-library-master/SSD1306.cpp
                ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/RPi-Pico-SSD1306-library-master/GFX.cpp
                ${CMAKE_CURRENT_LIST_DIR}/screens.cpp
)


target_include_directories(LED_LCD_i2c
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/dht22_Pico/
            ${CMAKE_CURRENT_LIST_DIR}/$ENV{PICO_LIB_PATH}/RPi-Pico-SSD1306-library-master/
)

# pull in common dependencies
target_link_libraries(LED_LCD_i2c pico_stdlib pico_stdio hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(LED_LCD_i2c)

# add url via pico_set_program_url
#example_auto_set_url(LED_LCD_i2c)

# Enable usb output, disable uart output
pico_enable_stdio_usb(LED_LCD_i2c 1)
pico_enable_stdio_uart(LED_LCD_i2c 0)
