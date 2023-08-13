/**
 * @author Aeldit
 * @link https://github.com/Aeldit
 */

//===============================================================================
/* Raspberry Pi PICO pinout

                        GP     PIN         PIN     GP
                                   +-----+
                             +-----|     |-----+
                        GP0  | 1   |     |  40 |  VBUS
                        GP1  | 2  #+-----+  39 |  VSYS
                        GND <| 3  GP25 LED  38 |> GND
       DIGITAL_INPUT    GP2  | 4            37 |  3V3_EN
        ANALOG_INPUT    GP3  | 5            36 |  3V3_OUT
                        GP4  | 6            35 |  ADC_VREF
                        GP5  | 7            34 |  GP28     BAR_1
                        GND <| 8            33 |> GND
                        GP6  | 9            32 |  GP27     BAR_2
                        GP7  | 10  +-----+  31 |  GP26     BAR_3
                        GP8  | 11  |     |  30 |  RUN
                        GP9  | 12  |     |  29 |  GP22     BAR_4
                        GND <| 13  +-----+  28 |> GND
                        GP10 | 14           27 |  GP21     BAR_5
                        GP11 | 15           26 |  GP20     BAR_6
                        GP12 | 16           25 |  GP19     BAR_7
                        GP13 | 17           24 |  GP18     BAR_8
                        GND <| 18           23 |> GND
                        GP14 | 19           22 |  GP17     BAR_9
                        GP15 | 20           21 |  GP16     BAR_10
                             +-----------------+
*/
//===============================================================================

//=============================================================
// INCLUDES
// ============================================================
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"

//=============================================================
// DEFINES
// ============================================================
#define LOW 0
#define HIGH 1

#define NB_BAR_LEDS 10

// ADC
//==============================
#define ADC_NUM 0
#define ADC_VREF 11
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1))

// PINS
//==============================
#define PIN_ANALOG_INPUT 26

#define PIN_BAR_10 16
#define PIN_BAR_9 17
#define PIN_BAR_8 18
#define PIN_BAR_7 19
#define PIN_BAR_6 20
#define PIN_BAR_5 21
#define PIN_BAR_4 22
#define PIN_BAR_3 2
#define PIN_BAR_2 27
#define PIN_BAR_1 28

// Tempos
//==============================
#define C_TIME_SOUND_ACQUISITION 10 * 1000 // 10ms : duration before next sound acquisition

//=============================================================
// CONSTANTS
// ============================================================
const uint8_t PINS_LED_BAR[NB_BAR_LEDS] = {PIN_BAR_10, PIN_BAR_9, PIN_BAR_8, PIN_BAR_7, PIN_BAR_6, PIN_BAR_5, PIN_BAR_4, PIN_BAR_3, PIN_BAR_2, PIN_BAR_1};

//=============================================================
// VARIABLES
// ============================================================
absolute_time_t timer_sound_sensor; // Timer for sound acquisition

uint8_t adc_value;
uint8_t adc_values[100]{0};

//=============================================================
// FUNCTIONS
// ============================================================

int main()
{
    stdio_init_all();

    // ADC
    adc_init();
    adc_gpio_init(PIN_ANALOG_INPUT);
    adc_select_input(ADC_NUM);

    // Built-in LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    // Bar LEDs
    for (int i = 0; i < NB_BAR_LEDS; i++)
    {
        gpio_init(PINS_LED_BAR[i]);
        gpio_set_dir(PINS_LED_BAR[i], GPIO_OUT);
    }

    timer_sound_sensor = get_absolute_time();

    while (true)
    {
        if (absolute_time_diff_us(timer_sound_sensor, get_absolute_time()) > C_TIME_SOUND_ACQUISITION)
        {
            adc_value = adc_read();
            printf("%d\n", adc_value);
            timer_sound_sensor = get_absolute_time();
        }

        /*for (int i = 0; i < NB_BAR_LEDS; i++)
        {
            gpio_put(PINS_LED_BAR[i], HIGH);
            sleep_ms(100);
            gpio_put(PINS_LED_BAR[i], LOW);
        }*/
    }
}
