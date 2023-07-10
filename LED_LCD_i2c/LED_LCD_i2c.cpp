/**
 * @file LED_LCD_i2c.c
 * @author Aeldit (raphael.roger.92@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-08-16
 *
 * @copyright Copyright (c) 2022
 *
 */

//=============================================================
// INCLUDES
// ============================================================
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <logo.hpp>
#include <GFX.hpp>

#include <dht22_pico.h>

#include "screens.h"

//=============================================================
// DEFINES
// ============================================================
// Debug
//==============================
// #define SERIAL_DEBUG
// #define SERIAL_INFOS

#define DEBUG_LIFE_LED

// Buttons indexes
//==============================
#define RED_LED 0
#define GREEN_LED 1
#define BLUE_LED 2
#define LIFE_LED 3

#define LCD_UP_BTN 0
#define LCD_DOWN_BTN 1
#define LCD_LEFT_BTN 2
#define LCD_RIGHT_BTN 3
#define LCD_ENTER_BTN 4

#define LCD_SCREEN_MAIN 0
#define LCD_SCREEN_DEBUG 1
#define LCD_SCREEN_TEMPERATURE 2

// PINs
//==============================
#define RED_LED_PIN 17
#define GREEN_LED_PIN 26
#define BLUE_LED_PIN 27
#define LIFE_LED_PIN 25 // Built-in LED

#define DHT22_PIN 15

#define DISP_SDA_PIN 12
#define DISP_SCL_PIN 13

#define LCD_UP_BTN_PIN 10
#define LCD_DOWN_BTN_PIN 9
#define LCD_LEFT_BTN_PIN 8
#define LCD_RIGHT_BTN_PIN 14

#define LCD_ENTER_BTN_PIN 11
#define LCD_CANCEL_BTN_PIN 7

// Tempos
//==============================
#define C_TIME_BUTTON_FILTER 30000       // 30ms : button filtering time
#define C_TIME_LIFE_LED_FILTER 1000000   // 1s : life LED filtering time
#define C_TIME_DHT_FILTER 2000000        // 2s : DHT filtering time
#define C_TIME_IS_LCD_ON 10000000        // 10s : duration before putting display asleep
#define C_TIME_LCD_BUTTONS_FILTER 250000 // 250ms : LEDs filtering time
#define C_TIME_LCD_SELECTOR 300000       // 300ms : duration before putting display asleep

#define C_COEFF_BUTTON_FILTER 4 // Buttons filtering coeff

//=============================================================
// CONSTANTS
// ============================================================
const uint8_t BUTTONS_PIN[5] = {
    LCD_UP_BTN_PIN, LCD_DOWN_BTN_PIN, LCD_LEFT_BTN_PIN, LCD_RIGHT_BTN_PIN, LCD_ENTER_BTN_PIN}; // Buttons PINs order (LEDs):  UP / DOWN / LEFT / RIGHT / ENTER

const uint8_t LEDS_PIN[4] = {RED_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN, LIFE_LED_PIN}; // LEDs PINs order (LEDs): RED / GREEN / BLUE / LIFE

const uint8_t NB_OPTIONS_ON_MENUS[2] = {2, 1}; // Screens order : main / debug

//=============================================================
// VARIABLES
// ============================================================
bool buttons_state[5] = {false, false, false, false, false}; // Buttons order: DOWN / UP / ENTER / CANCEL
bool leds_state[4] = {false, false, false, false};           // LEDs order: RED / GREEN / BLUE / LIFE

uint8_t buttons_counter[5] = {0, 0, 0, 0, 0}; // Counters used for buttons acquisition

absolute_time_t timer_buttons[5]; // Timer for buttons acquisition
absolute_time_t timer_life_led;   // Timer for life led blinking
absolute_time_t timer_led;        // Timer for buttons acquisition
absolute_time_t timer_dht;        // Timer for DHT acquisition
absolute_time_t timer_lcd;        // Timer for LCD
absolute_time_t timer_selector;   // Timer for LCD's selector
absolute_time_t timer_lcd_button; // Timer for LCD's buttons

// DHT
//==============================
uint8_t dht_status;
dht_reading dht;

float dht_temp;
float dht_hum;

bool is_dht_acquisition_done = false;

// LCD
//==============================
bool is_lcd_on = true;
bool is_display_update_needed = false;
bool is_current_screen_menu;
bool is_current_screen_toogle = true; // If the current screen allows the user to toogle an option

uint8_t menu_select = 0;   // Selected menu
uint8_t toogle_select = 1; // Selected option (OK or CANCEL)

uint8_t previous_screen = 0;       // Screens indexes order (starts at 0) : main menu / debug / temperature & humidity
uint8_t current_screen = 2;        // Screens indexes : same as above
uint8_t current_toogle_screen = 1; // Screens indexes : life LED / screen timeout

// Options
//==============================
// Variables names are separated in 3 parts:
//  - option_
//  - category_
//  - option_name
//==============================
bool option_debug_life_led = true;

bool option_lcd_do_screen_timeout = false;
int option_lcd_screen_timeout = 300000;

//=============================================================
// FUNCTIONS
// ============================================================
void buttons_acquisition();
void button_acquisition(uint8_t button);
void dht_acquisition();
void on_button_release(int button, int option);
void led_driving(uint8_t ledNumber);
void leds_driving();
void display_management(GFX *display);

int main()
{
    // Initialisation
    //=======================================================
    // Stdio
    //==============================
    stdio_init_all();

    // LEDs
    //==============================
    gpio_init(LEDS_PIN[RED_LED]);
    gpio_init(LEDS_PIN[GREEN_LED]);
    gpio_init(LEDS_PIN[BLUE_LED]);
    gpio_init(LEDS_PIN[LIFE_LED]);
    gpio_set_dir(LEDS_PIN[RED_LED], GPIO_OUT);
    gpio_set_dir(LEDS_PIN[GREEN_LED], GPIO_OUT);
    gpio_set_dir(LEDS_PIN[BLUE_LED], GPIO_OUT);
    gpio_set_dir(LEDS_PIN[LIFE_LED], GPIO_OUT);

    // Buttons
    //==============================
    gpio_init(BUTTONS_PIN[LCD_UP_BTN]);
    gpio_init(BUTTONS_PIN[LCD_DOWN_BTN]);
    gpio_init(BUTTONS_PIN[LCD_LEFT_BTN]);
    gpio_init(BUTTONS_PIN[LCD_RIGHT_BTN]);
    gpio_init(BUTTONS_PIN[LCD_ENTER_BTN]);

    gpio_set_dir(BUTTONS_PIN[LCD_UP_BTN], GPIO_IN);
    gpio_set_dir(BUTTONS_PIN[LCD_DOWN_BTN], GPIO_IN);
    gpio_set_dir(BUTTONS_PIN[LCD_LEFT_BTN], GPIO_IN);
    gpio_set_dir(BUTTONS_PIN[LCD_RIGHT_BTN], GPIO_IN);
    gpio_set_dir(BUTTONS_PIN[LCD_ENTER_BTN], GPIO_IN);

    gpio_set_pulls(BUTTONS_PIN[LCD_UP_BTN], true, false);
    gpio_set_pulls(BUTTONS_PIN[LCD_DOWN_BTN], true, false);
    gpio_set_pulls(BUTTONS_PIN[LCD_LEFT_BTN], true, false);
    gpio_set_pulls(BUTTONS_PIN[LCD_RIGHT_BTN], true, false);
    gpio_set_pulls(BUTTONS_PIN[LCD_ENTER_BTN], true, false);

    // DHT
    //==============================
    dht = dht_init(DHT22_PIN);

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

    timer_life_led = timer_led = timer_dht = timer_lcd = timer_selector = get_absolute_time();
    for (uint8_t i = 0; i < sizeof(BUTTONS_PIN); i++)
    {
        timer_buttons[i] = timer_life_led;
    }

    // Loop
    //=======================================================
    while (true)
    {
        // Life LED tempo (blinking the white LED)
        if (option_debug_life_led)
        {
            if (absolute_time_diff_us(timer_life_led, get_absolute_time()) > C_TIME_LIFE_LED_FILTER)
            {
                leds_state[LIFE_LED] = !leds_state[LIFE_LED];
                led_driving(LIFE_LED);
                timer_life_led = get_absolute_time();
            }
        }
        else // Shuts down the led if it was on
        {
            if (leds_state[LIFE_LED])
            {
                leds_state[LIFE_LED] = false;
                led_driving(LIFE_LED);
            }
        }

        // Buttons
        buttons_acquisition();

        // DHT
        dht_acquisition();

        // LCD
        display_management(&oled);
    }
}

// Buttons
//=======================================================
/**
 * @brief Acquires the button's state. Updates its state if it changes
 *
 */
void buttons_acquisition()
{
    for (uint8_t i = 0; i < sizeof(BUTTONS_PIN); i++)
    {
        button_acquisition(i);
    }
}

void button_acquisition(uint8_t button)
{
    if (absolute_time_diff_us(timer_buttons[button], get_absolute_time()) > C_TIME_BUTTON_FILTER)
    {
        if (gpio_get(BUTTONS_PIN[button]) == 0)
        {
            if (buttons_counter[button] < C_COEFF_BUTTON_FILTER)
            {
                buttons_counter[button] += 1;
            }
            else
            {
                buttons_state[button] = true;
            }
        }
        else
        {
            if (buttons_counter[button] > 0)
            {
                buttons_counter[button] -= 1;
            }
            else
            {
                buttons_state[button] = false;
            }
        }
        timer_buttons[button] = get_absolute_time();
    }
}

void on_button_release(int button, int option = 0)
{
    switch (button)
    {
        /*case LCD_UP_BTN:
            if (absolute_time_diff_us(timer_selector, get_absolute_time()) > C_TIME_LCD_SELECTOR)
            {
                if (is_current_screen_menu)
                {
                    if (menu_select < NB_OPTIONS_ON_MENUS[current_screen] && menu_select > 1)
                    {
                        menu_select--;
                    }
                    else
                    {
                        menu_select = NB_OPTIONS_ON_MENUS[current_screen];
                    }
                }
                timer_selector = get_absolute_time();
            }
            is_display_update_needed = true;
            break;

        case LCD_DOWN_BTN:
            if (absolute_time_diff_us(timer_selector, get_absolute_time()) > C_TIME_LCD_SELECTOR)
            {
                if (is_current_screen_menu)
                {
                    if (menu_select < NB_OPTIONS_ON_MENUS[current_screen] && menu_select > 1)
                    {
                        menu_select++;
                    }
                    else
                    {
                        menu_select = 0;
                    }
                }
                timer_selector = get_absolute_time();
            }
            is_display_update_needed = true;
            break;*/

    case LCD_ENTER_BTN:
        if (is_current_screen_toogle)
        {
            switch (option)
            {
            case 0:
                option_debug_life_led = !option_debug_life_led;
                break;

            case 1:
                option_lcd_do_screen_timeout = !option_lcd_do_screen_timeout;
                break;

            default:
                break;
            }
        }
        is_display_update_needed = true;
        break;

    case LCD_LEFT_BTN:
        if (is_current_screen_toogle)
        {
            current_screen = previous_screen;
        }
        is_display_update_needed = true;
        break;

    case LCD_RIGHT_BTN:
        if (is_current_screen_toogle)
        {
            if (toogle_select == 0)
            {
                toogle_select = 1;
            }
        }
        is_display_update_needed = true;
        break;
    }
}

// LEDs
//=======================================================
/**
 * @brief Changes the state of the LED (on or off)
 *
 * @param ledNumber The LED index
 */
void led_driving(uint8_t ledNumber)
{
    if (leds_state[ledNumber])
    {
        // Turns the LED on
        gpio_put(LEDS_PIN[ledNumber], 1);
    }
    else
    {
        // Turns the LED off
        gpio_put(LEDS_PIN[ledNumber], 0);
    }
}

/**
 * @brief Changes the state of all the RVB LEDs (on or off)
 *
 */
void leds_driving()
{
    for (uint8_t i = 0; i < 3; i++)
    {
        if (leds_state[i])
        {
            // Turns the LED on
            gpio_put(LEDS_PIN[i], 1);
        }
        else
        {
            // Turns the LED off
            gpio_put(LEDS_PIN[i], 0);
        }
    }
}

// DHT
//=======================================================
void dht_acquisition()
{
    if (absolute_time_diff_us(timer_dht, get_absolute_time()) > C_TIME_DHT_FILTER)
    {
        dht_status = dht_read(&dht);
        switch (dht_status)
        {
        case DHT_OK:
            dht_temp = dht.temp_celsius;
            dht_hum = dht.humidity;
            is_display_update_needed = true;
            break;

        case DHT_NO_SENSOR:
#ifdef SERIAL_INFOS
            printf("No sensor\n");
#endif
            break;

        case DHT_ERR_CHECKSUM:
#ifdef SERIAL_INFOS
            printf("Bad data (checksum)\n");
#endif
            break;

        default:
#ifdef SERIAL_INFOS
            printf("Bad data (NaN)\n");
#endif
            break;
        }

#ifdef SERIAL_DEBUG
        char temp_hum[16];
        sprintf(temp_hum, "T=%.1fC  H=%.1f%%\n", dht_temp, dht_hum);
        printf(temp_hum);
#endif
        timer_dht = get_absolute_time();
    }
}

// LCD
//=======================================================
void display_management(GFX *display)
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

    if (buttons_state[LCD_ENTER_BTN])
    {
        if (absolute_time_diff_us(timer_lcd_button, get_absolute_time()) > C_TIME_LCD_BUTTONS_FILTER)
        {
            if (is_lcd_on)
            {
                on_button_release(LCD_ENTER_BTN, 1);
                timer_lcd_button = get_absolute_time();
            }
            else
            {
                is_display_update_needed = true;
                timer_lcd_button = timer_lcd = get_absolute_time();
            }
        }
    }

    if (is_display_update_needed && !is_lcd_on)
    {
        is_lcd_on = true;
    }

    if (is_lcd_on)
    {
        if (option_lcd_do_screen_timeout)
        {
            // Put display asleep after timeout
            if (absolute_time_diff_us(timer_lcd, get_absolute_time()) > C_TIME_IS_LCD_ON)
            {
                is_lcd_on = false;
                is_display_update_needed = false;
                display->clear();
                display->display();
            }
        }
        if (is_display_update_needed)
        {
            /*switch (current_screen)
            {
            // Menus screens
            //====================
            case LCD_SCREEN_MAIN:
                is_current_screen_menu = true;
                screen_main_menu(display, menu_select);

            case LCD_SCREEN_DEBUG:
                is_current_screen_menu = true;
                screen_debug(display, menu_select);
                break;

            // Info screens
            //====================
            case LCD_SCREEN_TEMPERATURE:
                is_current_screen_menu = false;
                screen_temp_hum(display, dht_temp, dht_hum);
                break;

            default:
                break;
            }
        }*/
            char option[15] = "Screen Timeout";
            char Text[1];
            screen_option_toogle(display, option, option_lcd_do_screen_timeout, toogle_select);
            is_display_update_needed = false;
        }
    }
}
