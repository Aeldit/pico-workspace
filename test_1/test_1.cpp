/**
 * @author Aeldit
 * @link https://github.com/Aeldit
 */

//===============================================================================
/* Raspberry Pi PICO pinout

                        GP     PIN         PIN     GP
                                   +-----+
                             +-----|     |-----+
      BTN_LCD_CANCEL    GP0  | 1   |     |  40 |  VBUS
          BTN_LCD_UP    GP1  | 2  #+-----+  39 |  VSYS
                        GND <| 3  GP25 LED  38 |> GND
        BTN_LCD_DOWN    GP2  | 4            37 |  3V3_EN
       BTN_LCD_ENTER    GP3  | 5            36 |  3V3_OUT
                        GP4  | 6            35 |  ADC_VREF
                        GP5  | 7            34 |  GP28
                        GND <| 8            33 |> GND
                        GP6  | 9            32 |  GP27
                        GP7  | 10  +-----+  31 |  GP26
                        GP8  | 11  |     |  30 |  RUN
                        GP9  | 12  |     |  29 |  GP22
                        GND <| 13  +-----+  28 |> GND
                        GP10 | 14           27 |  GP21
                        GP11 | 15           26 |  GP20
                        GP12 | 16           25 |  GP19
               DHT22    GP13 | 17           24 |  GP18
                        GND <| 18           23 |> GND
            DISP_SDA    GP14 | 19           22 |  GP17
            DISP_SCL    GP15 | 20           21 |  GP16
                             +-----------------+
*/
//===============================================================================

//=============================================================
// INCLUDES
// ============================================================
#include <dht22_pico.h>
#include <stdio.h>

#include <GFX.hpp>
#include <logo.hpp>

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "screens.h"
#include "types.h"

//=============================================================
// DEFINES
// ============================================================
#define SERIAL_DEBUG

#define NB_LEDS 1
#define NB_BUTTONS 4

#define NB_SCREENS 3

// PINs
//==============================
#define PIN_BTN_LCD_CANCEL 0
#define PIN_BTN_LCD_UP 1
#define PIN_BTN_LCD_DOWN 2
#define PIN_BTN_LCD_ENTER 3

#define PIN_DHT22 13

#define PIN_DISP_SDA 14
#define PIN_DISP_SCL 15

#define PIN_LIFE_LED 25

// Indexes
//==============================
#define LED_LIFE 0

#define BTN_LCD_CANCEL 0
#define BTN_LCD_UP 1
#define BTN_LCD_DOWN 2
#define BTN_LCD_ENTER 3

#define SCREEN_MAIN 0
#define SCREEN_OPTIONS 1
#define SCREEN_TEMPERATURE 2

#define OPTION_LIFE_LED 0
#define OPTION_SCREEN_TIMEOUT 1
#define OPTION_USE_DHT 2

// Tempos
//==============================
#define C_TIME_BUTTON_FILTER 30 * 1000 // 30ms : button filtering time

#define C_TIME_LIFE_LED_OFF 3 * 1000 * 1000 // 3s : life LED OFF time
#define C_TIME_LIFE_LED_ON 100 * 1000       // 100ms : life LED ON time

#define C_TIME_LCD_SLEEP 5 * 1000 * 1000 // 5s : duration before putting display asleep
#define C_TIME_LCD_BUTTONS_FILTER 250000 // 250ms : LEDs filtering time
#define C_TIME_LCD_SELECTOR 300000       // 300ms : duration before putting display asleep

#define C_TIME_DHT_ACQUISITION 3 * 1000 * 1000 // 3s : Duration between 2 temperature acquisition

#define C_COEFF_BUTTON_FILTER 4 // Buttons filtering coeff

//=============================================================
// CONSTANTS
// ============================================================
const uint8_t BUTTONS_PINS[NB_BUTTONS] = {PIN_BTN_LCD_CANCEL, PIN_BTN_LCD_UP, PIN_BTN_LCD_DOWN,
                                          PIN_BTN_LCD_ENTER}; // Buttons PINs
const uint8_t LEDS_PINS[NB_LEDS] = {PIN_LIFE_LED};            // LEDs PINs

const uint8_t NB_AVAILABLE_SELECTORS[NB_SCREENS] = {NB_SCREENS - 1, NB_OPTIONS}; // Order : MAIN / OPTIONS

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
absolute_time_t timer_dht;                    // Timer for the DHT to acquire the temperature and the humidity

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

uint8_t selector = 0; // > 0 : list selection

uint8_t current_screen = SCREEN_MAIN;
uint8_t previous_screen = SCREEN_MAIN;

// DHT
//==============================
dht_reading dht;

bool is_dht_acquisition_in_progress;

t_temperature dht_temp = C_ERROR_DHT; // External temperature (sensor)
t_humidity dht_hum = C_ERROR_DHT;     // External humidity (sensor)

//=============================================================
// FUNCTIONS
// ============================================================
/**
 * @brief Updates the values inside the buttons_states array depending on which
 * buttons are pressed
 */
void button_acquisition(uint8_t button);
void buttons_acquisition();
/**
 * is_display_update_needed -> true
 * timer_lcd -> reset
 * event_button_updated -> false
 * pressed_button -> NB_BUTTONS;
 */
void reset_button_event();
/**
 * @brief Inverts the state of the LED (ON / OFF)
 *
 * @param ledNumber The LED index
 */
void led_driving(uint8_t ledNumber);
void display_management();
void temperature_management();

void init()
{
    // Stdio
    //==============================
    stdio_init_all();

    // LEDs
    //==============================
    gpio_init(PIN_LIFE_LED);

    gpio_set_dir(PIN_LIFE_LED, GPIO_OUT);

    // Buttons
    //==============================
    gpio_init(PIN_BTN_LCD_CANCEL);
    gpio_init(PIN_BTN_LCD_UP);
    gpio_init(PIN_BTN_LCD_DOWN);
    gpio_init(PIN_BTN_LCD_ENTER);

    gpio_set_dir(PIN_BTN_LCD_CANCEL, GPIO_IN);
    gpio_set_dir(PIN_BTN_LCD_UP, GPIO_IN);
    gpio_set_dir(PIN_BTN_LCD_DOWN, GPIO_IN);
    gpio_set_dir(PIN_BTN_LCD_ENTER, GPIO_IN);

    gpio_set_pulls(PIN_BTN_LCD_CANCEL, true, false);
    gpio_set_pulls(PIN_BTN_LCD_UP, true, false);
    gpio_set_pulls(PIN_BTN_LCD_DOWN, true, false);
    gpio_set_pulls(PIN_BTN_LCD_ENTER, true, false);

    // LCD
    //==============================
    i2c_init(i2c1, 400000);                         // Initialize I2C on i2c0 port with 400kHz
    gpio_set_function(PIN_DISP_SDA, GPIO_FUNC_I2C); // Use PIN_DISP_SDA as I2C
    gpio_set_function(PIN_DISP_SCL, GPIO_FUNC_I2C); // Use PIN_DISP_SCL as I2C
    gpio_pull_up(PIN_DISP_SDA);                     // Pull up PIN_DISP_SDA
    gpio_pull_up(PIN_DISP_SCL);                     // Pull up PIN_DISP_SCL

    oled = new GFX(0x3C, size::W128xH64, i2c1); // Declare oled instance
    oled->display(logo);

    // DHT
    //==============================
    dht = dht_init(PIN_DHT22);

    // Timers
    //==============================
    timer_life_led = timer_lcd = timer_dht = get_absolute_time();

    for (int i = 0; i < NB_BUTTONS; i++)
    {
        timer_buttons[i] = timer_life_led;
    }

    // Options
    //==============================
    options = (t_options){.names = {"Life LED", "Screen Timeout", "Use DHT Sensor"}, .values = {false, false, false}};
}

int main()
{
    init();

    while (true)
    {
        // Buttons
        buttons_acquisition();

        if (options.values[OPTION_LIFE_LED])
        {
            // The LED is turned ON for 100 ms and turned OFF for 3 s
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

        // DHT
        if (options.values[OPTION_USE_DHT])
        {
            temperature_management();
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

// LCD
//=======================================================
void display_management()
{
    if (event_button_updated && pressed_button != NB_BUTTONS)
    {
        switch (pressed_button)
        {
        case BTN_LCD_CANCEL:
            if (current_screen == SCREEN_OPTIONS || current_screen == SCREEN_TEMPERATURE)
            {
                previous_screen = current_screen = SCREEN_MAIN;
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

        case BTN_LCD_ENTER:
            if (is_lcd_on)
            {
                switch (current_screen)
                {
                case SCREEN_MAIN:
                    if (selector >= 0 && selector < NB_SCREENS - 1)
                    {
                        current_screen = selector + 1;
                        selector = 0;

                        if (current_screen == SCREEN_TEMPERATURE && !options.values[OPTION_USE_DHT])
                        {
                            char Text[19];
                            sprintf(Text, "DHT not initialized");
                            draw_alert_header(oled, Text, 19);
                            current_screen--;
                        }
                    }
                    break;

                case SCREEN_OPTIONS:
                    if (selector >= 0 && selector < NB_OPTIONS)
                    {
                        options.values[selector] = !options.values[selector];
                    }
                    break;

                default:
                    break;
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
            switch (current_screen)
            {
            case SCREEN_MAIN:
                screen_main_menu(oled, selector);
                break;

            case SCREEN_OPTIONS:
                screen_options(oled, selector, &options, sizeof(options.values));
                break;

            case SCREEN_TEMPERATURE:
                if (previous_screen == current_screen)
                {
                    screen_temp_hum(oled, dht.temp_celsius, dht.humidity, true);
                }
                else
                {
                    screen_temp_hum(oled, dht.temp_celsius, dht.humidity, false);
                }
            }
            is_display_update_needed = false;
        }
    }
}

// DHT
//=======================================================
void temperature_management()
{
    uint8_t dht_status;

    if (absolute_time_diff_us(timer_dht, get_absolute_time()) > C_TIME_DHT_ACQUISITION)
    {
        if (!is_dht_acquisition_in_progress)
        {
            dht_status = dht_read(&dht);

            if (dht_status == DHT_OK)
            {
                dht_temp = dht.temp_celsius;
                dht_hum = dht.humidity;
            }

            is_dht_acquisition_in_progress = true;
        }
        else
        {
            is_dht_acquisition_in_progress = false;
            is_display_update_needed = true;
        }
        timer_dht = get_absolute_time();
    }
}
