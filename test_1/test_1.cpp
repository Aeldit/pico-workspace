/**
 * @file test_1.cpp
 * @author Aeldit (raphael.roger.92@gmail.com)
 * @copyright Copyright (c) 2022
 */

//===============================================================================
/* Raspberry Pi PICO pinout

                        GP     PIN         PIN     GP
                                   +-----+
                             +-----|     |-----+
                        GP0  | 1   |     |  40 | VBUS
                        GP1  | 2  #+-----+  39 | VSYS
                        GND <| 3  GP25 LED  38 |>GND
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
            DISP_SDA    GP14 | 19           22 | GP17    BTN_LCD_ENTER
            DISP_SCL    GP15 | 20           21 | GP16    BTN_LIFE
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

#include "screens.h"

//=============================================================
// DEFINES
// ============================================================
#define SERIAL_DEBUG

#define NB_LEDS 1
#define NB_BUTTONS 2

// PINs
//==============================
#define DISP_SDA_PIN 14
#define DISP_SCL_PIN 15

#define PIN_LIFE_BTN 16
#define PIN_BTN_LCD_ENTER 17

#define PIN_LIFE_LED 25

// Indexes
//==============================
#define LIFE_LED 0

#define BTN_LIFE 0
#define BTN_LCD_ENTER 1

// Tempos
//==============================
#define C_TIME_BUTTON_FILTER 30000     // 30ms : button filtering time
#define C_TIME_LIFE_LED_FILTER 1000000 // 1s : life LED filtering time

#define C_TIME_IS_LCD_ON 10000000        // 10s : duration before putting display asleep
#define C_TIME_LCD_BUTTONS_FILTER 250000 // 250ms : LEDs filtering time
#define C_TIME_LCD_SELECTOR 300000       // 300ms : duration before putting display asleep

#define C_COEFF_BUTTON_FILTER 4 // Buttons filtering coeff

//=============================================================
// CONSTANTS
// ============================================================
const uint8_t BUTTONS_PINS[NB_BUTTONS] = {PIN_LIFE_BTN, PIN_BTN_LCD_ENTER}; // Buttons PINs order for LEDs:
const uint8_t LEDS_PINS[NB_LEDS] = {PIN_LIFE_LED};                          // LEDs PINs order for LEDs:

//=============================================================
// VARIABLES
// ============================================================
bool buttons_states[NB_BUTTONS] = {false, false};          // Buttons order:
bool previous_buttons_states[NB_BUTTONS] = {false, false}; // Same as above
bool leds_states[NB_LEDS] = {true};                        // LEDs order:

uint8_t buttons_counters[NB_BUTTONS] = {0, 0}; // Counters used for buttons acquisition

absolute_time_t timer_button;     // Timer for buttons acquisition
absolute_time_t timer_life_led;   // Timer for life led blinking
absolute_time_t timer_lcd;        // Timer for the LCD to shutdown
absolute_time_t timer_lcd_button; // Timer for the LCD button

// Events
//==============================
bool event_button_updated = false;

// Options
//==============================
bool option_debug_life_led = true;

// LCD
//==============================
GFX *oled;

bool is_lcd_on = true;
bool is_display_update_needed = false;

uint8_t selected_menu = 0;   // Selected menu
uint8_t selected_toggle = 1; // Selected option (OK or CANCEL)

//=============================================================
// FUNCTIONS
// ============================================================
/**
 * @brief Updates the values inside the buttons_states array depending on which buttons are pressed
 */
void button_acquisition(uint8_t button);
void buttons_acquisition();
void on_button_release(uint8_t button);
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
void display_management();

int main()
{
    // Initialisation
    //=======================================================
    // Stdio
    //==============================
    stdio_init_all();

    // LEDs
    //==============================
    gpio_init(PIN_LIFE_LED);

    gpio_set_dir(PIN_LIFE_LED, GPIO_OUT);

    // Buttons
    //==============================
    gpio_init(PIN_LIFE_BTN);
    gpio_init(PIN_BTN_LCD_ENTER);

    gpio_set_dir(PIN_LIFE_BTN, GPIO_IN);
    gpio_set_dir(PIN_BTN_LCD_ENTER, GPIO_IN);

    gpio_set_pulls(PIN_LIFE_BTN, true, false);
    gpio_set_pulls(PIN_BTN_LCD_ENTER, true, false);

    // LCD
    //==============================
    i2c_init(i2c1, 400000);                         // Initialize I2C on i2c0 port with 400kHz
    gpio_set_function(DISP_SDA_PIN, GPIO_FUNC_I2C); // Use DISP_SDA_PIN as I2C
    gpio_set_function(DISP_SCL_PIN, GPIO_FUNC_I2C); // Use DISP_SCL_PIN as I2C
    gpio_pull_up(DISP_SDA_PIN);                     // Pull up DISP_SDA_PIN
    gpio_pull_up(DISP_SCL_PIN);                     // Pull up DISP_SCL_PIN
    //  LCD Init
    oled = new GFX(0x3C, size::W128xH64, i2c1); // Declare oled instance
    oled->display(logo);

    // Timers
    //==============================
    timer_life_led = timer_button = timer_lcd = get_absolute_time();

    // Loop
    //=======================================================
    while (true)
    {
        // Life LED tempo (blinking the white LED)
        /*if (option_debug_life_led)
        {
            if (absolute_time_diff_us(timer_life_led, get_absolute_time()) > C_TIME_LIFE_LED_FILTER)
            {
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

        if (event_button_updated)
        {
            event_button_updated = false;

#ifdef SERIAL_DEBUG
            printf("Button pressed");
#endif
            if (buttons_states[BTN_LIFE])
            {
                on_button_release(BTN_LIFE);
            }
        }

        // LCD
        // display_management();
    }
}

// Buttons
//=======================================================

void button_acquisition(uint8_t button)
{
    if (absolute_time_diff_us(timer_button, get_absolute_time()) > C_TIME_BUTTON_FILTER)
    {
        if (gpio_get(BUTTONS_PINS[button]) == 0)
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
        timer_button = get_absolute_time();

        if (previous_buttons_states[button] != buttons_states[button])
        {
            previous_buttons_states[button] = buttons_states[button];
            event_button_updated = true;
        }
    }
}

void buttons_acquisition()
{
    for (uint8_t i = 0; i < NB_BUTTONS; i++)
    {
        button_acquisition(i);
    }
}

void on_button_release(uint8_t button)
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

// LCD
//=======================================================
void display_management()
{
    // TODO -> make work Turns on the LCD if a button is pressed (and if the LCD was turned off)
    /*if (!is_lcd_on)
    {
        if (buttons_state[LCD_DOWN_BTN] || buttons_state[LCD_UP_BTN] || buttons_state[LCD_LEFT_BTN] || buttons_state[LCD_RIGHT_BTN] || buttons_state[LCD_ENTER_BTN])
        {
            is_display_update_needed = true;
            timer_lcd = get_absolute_time();
        }
    }*/

    /*if (buttons_states[BTN_LCD_ENTER])
    {
        if (absolute_time_diff_us(timer_lcd_button, get_absolute_time()) > C_TIME_LCD_BUTTONS_FILTER)
        {
            if (is_lcd_on)
            {
                on_button_press(BTN_LCD_ENTER);
                timer_lcd_button = get_absolute_time();
            }
            else
            {
                is_display_update_needed = true;
                timer_lcd_button = timer_lcd = get_absolute_time();
            }
        }
    }*/

    if (is_display_update_needed && !is_lcd_on)
    {
        is_lcd_on = true;
    }

    if (is_lcd_on)
    {
        oled->clear();
        oled->display();
        // Put display asleep after timeout
        /*if (absolute_time_diff_us(timer_lcd, get_absolute_time()) > C_TIME_IS_LCD_ON)
        {
            is_lcd_on = false;
            is_display_update_needed = false;
            oled->clear();
            oled->display();
        }

        if (is_display_update_needed)
        {
            char option[15] = "Screen Timeout";
            char Text[1];
            screen_option_toggle(oled, option, option_debug_life_led, selected_toggle);
            is_display_update_needed = false;
        }*/
    }
}
