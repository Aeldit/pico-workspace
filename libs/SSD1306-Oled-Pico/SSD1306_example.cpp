#include <GFX.hpp>
#include <logo.hpp>
#include <stdint.h>
#include <stdio.h>

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define SSD1306_SDA_PIN 16
#define SSD1306_SCL_PIN 17

GFX *oled; // Declare oled instance
char Text[20];

int main()
{
    stdio_init_all();

    // I2C configuration
    i2c_init(i2c0, 400000); // Initialize I2C on i2c0 port with 400kHz
    gpio_set_function(SSD1306_SDA_PIN, GPIO_FUNC_I2C); // Use DISP_SDA_PIN as
                                                       // I2C
    gpio_set_function(SSD1306_SCL_PIN, GPIO_FUNC_I2C); // Use DISP_SCL_PIN as
                                                       // I2C
    gpio_pull_up(SSD1306_SDA_PIN); // Pull up DISP_SDA_PIN
    gpio_pull_up(SSD1306_SCL_PIN); // Pull up DISP_SCL_PIN

    // SSD1306 Oled construction
    oled = new GFX(SSD1306_I2C_ADDR, size::W128xH64,
                   i2c0); // Declare oled instance
    oled->display(logo); // Display bitmap

    sleep_ms(1000);

    while (1)
    {
        // Contrast
        oled->setContrast(0);
        sleep_ms(1000);
        oled->setContrast(0xFF);
        sleep_ms(1000);

        // Rotation
        oled->rotateDisplay(0);
        sleep_ms(1000);
        oled->rotateDisplay(1);
        sleep_ms(1000);

        // Color inversion
        oled->invertColors(1);
        sleep_ms(1000);
        oled->invertColors(0);
        sleep_ms(1000);

        // Scroll
        oled->horizontal_scroll(true, 0x00, 0x07, SSD1306_SCROLLSPEED_2);
        sleep_ms(2000);
        oled->stop_scroll();
        oled->horizontal_scroll(false, 0x00, 0x07, SSD1306_SCROLLSPEED_2);
        sleep_ms(2000);
        oled->stop_scroll();
        oled->vertical_horizontal_scroll(true, 0x00, 0x07,
                                         SSD1306_SCROLLSPEED_2, 1);
        sleep_ms(2000);
        oled->stop_scroll();

        // Text
        oled->clear();
        for (uint8_t i = 0; i < 8; i++)
        {
            sprintf(Text, "Line %u : Text Text", i);
            oled->drawString(0, 8 * i, Text);
        }
        oled->display();
    }

    return 0;
}