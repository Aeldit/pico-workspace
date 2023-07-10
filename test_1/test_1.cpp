/**
 * @file test_1.cpp
 * @author Aeldit (raphael.roger.92@gmail.com)
 *
 * @copyright Copyright (c) 2022
 */

//===============================================================================
/* Raspberry Pi PICO pinout

                        GP     PIN         PIN   GP
                                   +-----+
                             +-----|     |-----+
                        GP0  | 1   |     |  40 | VBUS
                        GP1  | 2  #+-----+  39 | VSYS
                        GND <| 3  GP25 Led  38 |>GND
                        GP2  | 4            37 | 3V3_EN
                        GP3  | 5            36 | 3V3_OUT
                        GP4  | 6            35 | ADC_VREF
                        GP5  | 7            34 | GP28
                        GND <| 8            33 |>GND
                        GP6  | 9            32 | GP27
                        GP7  | 10  +-----+  31 | GP26
                        GP8  | 11  |     |  30 | RUN
                        GP9  | 12  |     |  29 | GP22
                        GND <| 13  +-----+  28 |>GND
                        GP10 | 14           27 | GP21
                        GP11 | 15           26 | GP20
                        GP12 | 16           25 | GP19
                        GP13 | 17           24 | GP18
                        GND <| 18           23 |>GND
            DISP_SDA    GP14 | 19           22 | GP17
            DISP_SCL    GP15 | 20           21 | GP16    LIFE_BTN
                             +-----------------+
*/
//===============================================================================

//=============================================================
// INCLUDES
// ============================================================
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include <GFX.hpp>
#include <logo.hpp>

//=============================================================
// DEFINES
// ============================================================
#define SERIAL_DEBUG

// PINs
//==============================
#define DISP_SDA_PIN 14
#define DISP_SCL_PIN 15

#define LIFE_BTN_PIN 16

#define LIFE_LED_PIN 25

// Indexes
//==============================
#define LIFE_LED 0

#define BTN_LIFE 0

// Tempos
//==============================
#define C_TIME_BUTTON_FILTER 30000     // 30ms : button filtering time
#define C_TIME_LIFE_LED_FILTER 1000000 // 1s : life LED filtering time

#define C_COEFF_BUTTON_FILTER 4 // Buttons filtering coeff

//=============================================================
// CONSTANTS
// ============================================================
const uint8_t BUTTONS_PINS[1] = {LIFE_BTN_PIN}; // Buttons PINs order for LEDs:
const uint8_t LEDS_PINS[1] = {LIFE_LED_PIN};    // LEDs PINs order for LEDs:

//=============================================================
// VARIABLES
// ============================================================
bool buttons_states[1] = {false}; // Buttons order: DOWN / UP / ENTER / CANCEL
bool leds_states[1] = {false};    // LEDs order: RED / GREEN / BLUE / LIFE

uint8_t buttons_counters[1] = {0}; // Counters used for buttons acquisition

absolute_time_t timer_buttons[1]; // Timer for buttons acquisition
absolute_time_t timer_life_led;   // Timer for life led blinking

// Options
//==============================
bool option_debug_life_led = true;

// LCD
//==============================
bool is_lcd_on = true;

//=============================================================
// FUNCTIONS
// ============================================================
/**
 * @brief Updates the values inside the buttons_states array depending on which buttons are pressed
 */
void button_acquisition(uint8_t button);
void buttons_acquisition();
void on_button_press(uint8_t button);
/**
 * @brief Changes the state of the LED (ON or OFF)
 *
 * @param ledNumber The LED index
 */
void led_driving(uint8_t ledNumber);
/**
 * @brief Changes the state of all the RVB LEDs (on or off)
 */
void leds_driving();

int main()
{
    // Initialisation
    //=======================================================
    // Stdio
    //==============================
    stdio_init_all();

    // LEDs
    //==============================
    gpio_init(LIFE_LED_PIN);

    gpio_set_dir(LIFE_LED_PIN, GPIO_OUT);

    // Buttons
    //==============================
    gpio_init(LIFE_BTN_PIN);

    gpio_set_dir(LIFE_BTN_PIN, GPIO_IN);

    gpio_set_pulls(LIFE_BTN_PIN, true, false);

    // LCD
    //==============================
    i2c_init(i2c0, 400000);                         // Initialize I2C on i2c0 port with 400kHz
    gpio_set_function(DISP_SDA_PIN, GPIO_FUNC_I2C); // Use DISP_SDA_PIN as I2C
    gpio_set_function(DISP_SCL_PIN, GPIO_FUNC_I2C); // Use DISP_SCL_PIN as I2C
    gpio_pull_up(DISP_SDA_PIN);                     // Pull up DISP_SDA_PIN
    gpio_pull_up(DISP_SCL_PIN);                     // Pull up DISP_SCL_PIN
    // LCD Init
    GFX oled(0x3C, size::W128xH64, i2c0); // Declare oled instance
    oled.display(logo);

    // Timers
    //==============================
    timer_life_led = get_absolute_time();
    for (uint8_t i = 0; i < sizeof(BUTTONS_PINS); i++)
    {
        timer_buttons[i] = timer_life_led;
    }

    // Loop
    //=======================================================
    while (true)
    {
        // Life LED tempo (blinking the white LED)
        /*if (option_debug_life_led)
        {
            if (absolute_time_diff_us(timer_life_led, get_absolute_time()) > C_TIME_LIFE_LED_FILTER)
            {
                leds_states[LIFE_LED] = !leds_states[LIFE_LED];
                led_driving(LIFE_LED);
                timer_life_led = get_absolute_time();
            }
        }
        else // Shuts down the led if it was on
        {
            if (leds_states[LIFE_LED])
            {
                leds_states[LIFE_LED] = false;
                led_driving(LIFE_LED);
            }
        }*/

        // Buttons
        buttons_acquisition();

        if (buttons_states[BTN_LIFE])
        {
            on_button_press(BTN_LIFE);
        }
    }
}

// Buttons
//=======================================================

void button_acquisition(uint8_t button)
{
    if (absolute_time_diff_us(timer_buttons[button], get_absolute_time()) > C_TIME_BUTTON_FILTER)
    {
        if (gpio_get(BUTTONS_PINS[button]) != 0)
        {
            if (buttons_counters[button] < C_COEFF_BUTTON_FILTER)
            {
                buttons_counters[button] += 1;
            }
            else
            {
                buttons_states[button] = true;
            }
        }
        else
        {
            if (buttons_counters[button] > 0)
            {
                buttons_counters[button] -= 1;
            }
            else
            {
                buttons_states[button] = false;
            }
        }
        timer_buttons[button] = get_absolute_time();
    }
}

void buttons_acquisition()
{
    for (uint8_t i = 0; i < sizeof(BUTTONS_PINS); i++)
    {
        button_acquisition(i);
    }
}

void on_button_press(uint8_t button)
{
    switch (button)
    {
    case BTN_LIFE:
        led_driving(LIFE_LED);
        break;
    }
}

// LEDs
//=======================================================

void led_driving(uint8_t ledNumber)
{
    if (leds_states[ledNumber])
    {
        gpio_put(LEDS_PINS[ledNumber], 1); // Turns the LED ON
    }
    else
    {
        gpio_put(LEDS_PINS[ledNumber], 0); // Turns the LED OFF
    }
    leds_states[ledNumber] = !leds_states[ledNumber];
}

void leds_driving()
{
    for (uint8_t i = 0; i < sizeof(LEDS_PINS); i++)
    {
        led_driving(i);
    }
}
