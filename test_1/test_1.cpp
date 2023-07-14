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
      BTN_LCD_CANCEL    GP0  | 1   |     |  40 | VBUS
          BTN_LCD_UP    GP1  | 2  #+-----+  39 | VSYS
                        GND <| 3  GP25 LED  38 |>GND
        BTN_LCD_DOWN    GP2  | 4            37 | 3V3_EN
       BTN_LCD_ENTER    GP3  | 5            36 | 3V3_OUT
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
#include "types.h"

#include <array>
#include <string>
using std::array;
using std::string;

//=============================================================
// DEFINES
// ============================================================
#define SERIAL_DEBUG

#define NB_LEDS 1
#define NB_BUTTONS 5

#define NB_MENUS 2
#define NB_OPTIONS 2

// PINs
//==============================
#define PIN_BTN_LCD_CANCEL 0
#define PIN_BTN_LCD_UP 1
#define PIN_BTN_LCD_DOWN 2
#define PIN_BTN_LCD_ENTER 3

#define PIN_DISP_SDA 14
#define PN_DISP_SCL 15

#define PIN_BTN_LIFE 16

#define PIN_LIFE_LED 25

// Indexes
//==============================
#define LED_LIFE 0

#define BTN_LIFE 0
#define BTN_LCD_CANCEL 1
#define BTN_LCD_UP 2
#define BTN_LCD_DOWN 3
#define BTN_LCD_ENTER 4

#define SCREEN_MAIN 0
#define SCREEN_OPTIONS 1

#define OPTION_LIFE_LED 0
#define OPTION_SCREEN_TIMEOUT 1

// Tempos
//==============================
#define C_TIME_BUTTON_FILTER 30 * 1000  // 30ms : button filtering time
#define C_TIME_LIFE_LED_OFF 3000 * 1000 // 5s : life LED OFF time
#define C_TIME_LIFE_LED_ON 100 * 1000   // 3s : life LED ON time

#define C_TIME_LCD_SLEEP 5 * 1000 * 1000 // 5s : duration before putting display asleep
#define C_TIME_LCD_BUTTONS_FILTER 250000 // 250ms : LEDs filtering time
#define C_TIME_LCD_SELECTOR 300000       // 300ms : duration before putting display asleep

#define C_COEFF_BUTTON_FILTER 4 // Buttons filtering coeff

//=============================================================
// CONSTANTS
// ============================================================
const uint8_t BUTTONS_PINS[NB_BUTTONS] = {
    PIN_BTN_LIFE,
    PIN_BTN_LCD_CANCEL, PIN_BTN_LCD_UP, PIN_BTN_LCD_DOWN, PIN_BTN_LCD_ENTER}; // Buttons PINs
const uint8_t LEDS_PINS[NB_LEDS] = {PIN_LIFE_LED};                            // LEDs PINs

const uint8_t NB_AVAILABLE_SELECTORS[NB_MENUS] = {2, 2}; // Order : MAIN / OPTIONS

//=============================================================
// VARIABLES
// ============================================================
bool buttons_states[NB_BUTTONS]{false};          // Buttons order:
bool previous_buttons_states[NB_BUTTONS]{false}; // Same as above
bool leds_states[NB_LEDS]{false};                // LEDs order:

uint8_t buttons_counters[NB_BUTTONS]{0}; // Counters used for buttons acquisition

uint8_t pressed_button = NB_BUTTONS;

absolute_time_t timer_buttons[NB_BUTTONS]{0}; // Timer for buttons acquisition
absolute_time_t timer_life_led;               // Timer for life led blinking
absolute_time_t timer_lcd;                    // Timer for the LCD to shutdown

// Events
//==============================
bool event_button_updated = false;

// Options
//==============================
t_options options;

// LCD
//==============================
GFX *oled;

bool is_lcd_on = true;
bool is_display_update_needed = false;
bool is_screen_menu;

int selector = 0; // > 0 : list selection | -1 : previous page button in header

uint8_t previous_screen = SCREEN_MAIN;
uint8_t current_screen = SCREEN_MAIN;

//=============================================================
// FUNCTIONS
// ============================================================
/**
 * @brief Updates the values inside the buttons_states array depending on which buttons are pressed
 */
void button_acquisition(uint8_t button);
void buttons_acquisition();
bool on_button_release(uint8_t button);
void reset_button_event();
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
    gpio_init(PIN_BTN_LIFE);
    gpio_init(PIN_BTN_LCD_CANCEL);
    gpio_init(PIN_BTN_LCD_UP);
    gpio_init(PIN_BTN_LCD_DOWN);
    gpio_init(PIN_BTN_LCD_ENTER);

    gpio_set_dir(PIN_BTN_LIFE, GPIO_IN);
    gpio_set_dir(PIN_BTN_LCD_CANCEL, GPIO_IN);
    gpio_set_dir(PIN_BTN_LCD_UP, GPIO_IN);
    gpio_set_dir(PIN_BTN_LCD_DOWN, GPIO_IN);
    gpio_set_dir(PIN_BTN_LCD_ENTER, GPIO_IN);

    gpio_set_pulls(PIN_BTN_LIFE, true, false);
    gpio_set_pulls(PIN_BTN_LCD_CANCEL, true, false);
    gpio_set_pulls(PIN_BTN_LCD_UP, true, false);
    gpio_set_pulls(PIN_BTN_LCD_DOWN, true, false);
    gpio_set_pulls(PIN_BTN_LCD_ENTER, true, false);

    // LCD
    //==============================
    i2c_init(i2c1, 400000);                         // Initialize I2C on i2c0 port with 400kHz
    gpio_set_function(PIN_DISP_SDA, GPIO_FUNC_I2C); // Use PIN_DISP_SDA as I2C
    gpio_set_function(PN_DISP_SCL, GPIO_FUNC_I2C);  // Use PN_DISP_SCL as I2C
    gpio_pull_up(PIN_DISP_SDA);                     // Pull up PIN_DISP_SDA
    gpio_pull_up(PN_DISP_SCL);                      // Pull up PN_DISP_SCL

    oled = new GFX(0x3C, size::W128xH64, i2c1); // Declare oled instance
    oled->display(logo);

    // Timers
    //==============================
    timer_life_led = timer_lcd = get_absolute_time();
    for (int i = 0; i < NB_BUTTONS; i++)
    {
        timer_buttons[i] = timer_life_led;
    }

    options = (t_options){.options_names = {"Life LED", "Screen Timeout"}, .values = {false, true}};

    // Loop
    //=======================================================
    while (true)
    {
        // Buttons
        buttons_acquisition();

        if (options.values[OPTION_LIFE_LED])
        {
            if (leds_states[LED_LIFE])
            {
                if (absolute_time_diff_us(timer_life_led, get_absolute_time()) > C_TIME_LIFE_LED_ON)
                {
                    led_driving(LED_LIFE);
                    timer_life_led = get_absolute_time();
                }
            }
            else
            {
                if (absolute_time_diff_us(timer_life_led, get_absolute_time()) > C_TIME_LIFE_LED_OFF)
                {
                    led_driving(LED_LIFE);
                    timer_life_led = get_absolute_time();
                }
            }
        }
        else
        {
            // Turns the LED OFF if the OPTION_LIFE_LED is false
            if (leds_states[LED_LIFE])
            {
                led_driving(LED_LIFE);
            }
        }

        // LCD
        display_management();
    }
}

// Buttons
//=======================================================

void button_acquisition(uint8_t button)
{
    if (absolute_time_diff_us(timer_buttons[button], get_absolute_time()) > C_TIME_BUTTON_FILTER)
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
        timer_buttons[button] = get_absolute_time();

        if (previous_buttons_states[button] != buttons_states[button])
        {
            previous_buttons_states[button] = buttons_states[button];
            event_button_updated = true;

            if (buttons_states[button])
            {
                pressed_button = button;
            }
            else
            {
                pressed_button = NB_BUTTONS;
            }
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

bool on_button_release(uint8_t button)
{
    bool found = false;

    switch (button)
    {
    case BTN_LIFE:
        led_driving(LED_LIFE);
        found = true;
        break;
    }
    return found;
}

void reset_button_event()
{
    is_display_update_needed = true;
    timer_lcd = get_absolute_time();
    event_button_updated = false;
    pressed_button = NB_BUTTONS;
}

// LEDs
//=======================================================

void led_driving(uint8_t ledNumber)
{
    if (leds_states[ledNumber])
    {
        gpio_put(LEDS_PINS[ledNumber], 0); // Turns the LED OFF
    }
    else
    {
        gpio_put(LEDS_PINS[ledNumber], 1); // Turns the LED ON
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
    if (event_button_updated && pressed_button != NB_BUTTONS)
    {
        switch (pressed_button)
        {
        case BTN_LCD_CANCEL:
            if (current_screen != SCREEN_MAIN)
            {
                selector = -1;
            }
            reset_button_event();
            break;

        case BTN_LCD_DOWN:
            if (selector < NB_AVAILABLE_SELECTORS[current_screen] - 1)
            {
                selector++;
            }
            else
            {
                selector = 0;
            }
            reset_button_event();
            break;

        case BTN_LCD_UP:
            if (selector > 0)
            {
                selector--;
            }
            else
            {
                selector = NB_AVAILABLE_SELECTORS[current_screen] - 1;
            }
            reset_button_event();
            break;

        case BTN_LCD_ENTER:
            if (current_screen == SCREEN_MAIN)
            {
                if (selector >= 0 && selector < NB_MENUS)
                {
                    current_screen++;
                }
            }
            else if (current_screen == SCREEN_OPTIONS)
            {
                if (selector >= 0 && selector < NB_OPTIONS)
                {
                    options.values[selector] = !options.values[selector];
                }
                else if (selector == -1)
                {
                    current_screen = previous_screen;
                    selector = 0;
                }
            }
            reset_button_event();
            break;
        }
    }

    if (is_display_update_needed && !is_lcd_on)
    {
        is_lcd_on = true;
        oled->display();
    }

    if (is_lcd_on)
    {
        if (options.values[OPTION_SCREEN_TIMEOUT])
        {
            if (absolute_time_diff_us(timer_lcd, get_absolute_time()) > C_TIME_LCD_SLEEP)
            {
                is_lcd_on = false;
                oled->clear();
                oled->display();
            }
        }

        if (is_display_update_needed)
        {
            /*if (previous_screen != current_screen)
            {
                previous_screen = current_screen;
            }*/

            switch (current_screen)
            {
            case SCREEN_MAIN:
                screen_main_menu(oled, selector);
                break;

            case SCREEN_OPTIONS:
                screen_options(oled, selector, &options, sizeof(options.values));
                break;
            }
            is_display_update_needed = false;
        }
    }
}
