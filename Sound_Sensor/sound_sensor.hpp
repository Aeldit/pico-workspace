/**
 * @author Aeldit
 * @link https://github.com/Aeldit
 */

#ifndef SOUND_SENSOR_H
#define SOUND_SENSOR_H
//=============================================================================
/* Raspberry Pi PICO pinout

                        GP     PIN         PIN     GP
                                   +-----+
                             +-----|     |-----+
                        GP0  | 1   |     |  40 |  VBUS
                        GP1  | 2  #+-----+  39 |  VSYS
                        GND <| 3  GP25 LED  38 |> GND
               BAR_1    GP2  | 4            37 |  3V3_EN
                        GP3  | 5            36 |  3V3_OUT
                        GP4  | 6            35 |  ADC_VREF
                        GP5  | 7            34 |  GP28     BAR_10
                        GND <| 8            33 |> GND
                        GP6  | 9            32 |  GP27     BAR_9
                        GP7  | 10  +-----+  31 |  GP26     ANALOG_INPUT
                        GP8  | 11  |     |  30 |  RUN
                        GP9  | 12  |     |  29 |  GP22     BAR_8
                        GND <| 13  +-----+  28 |> GND
                        GP10 | 14           27 |  GP21     BAR_7
                        GP11 | 15           26 |  GP20     BAR_6
                        GP12 | 16           25 |  GP19     BAR_5
                        GP13 | 17           24 |  GP18     BAR_6
                        GND <| 18           23 |> GND
                        GP14 | 19           22 |  GP17     BAR_3
                        GP15 | 20           21 |  GP16     BAR_2
                             +-----------------+
*/
//=============================================================================
// INCLUDES
// ============================================================================
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"

//=============================================================================
// DEFINES
// ============================================================================
#define LOW 0
#define HIGH 1

#define NB_BAR_LEDS 10

#define ADC_ARRAY_LENGHT 100

// ADC
//==============================
#define ADC_NUM 0
#define ADC_VREF 11
// #define ADC_RANGE (1 << 12)
// #define ADC_CONVERT ADC_VREF / (ADC_RANGE - 1)

// PINS
//==============================
#define PIN_ANALOG_INPUT 26

#define PIN_BAR_1 2
#define PIN_BAR_2 16
#define PIN_BAR_3 17
#define PIN_BAR_4 18
#define PIN_BAR_5 19
#define PIN_BAR_6 20
#define PIN_BAR_7 21
#define PIN_BAR_8 22
#define PIN_BAR_9 27
#define PIN_BAR_10 28

// Tempos
//==============================
#define C_TIME_SOUND_ACQUISITION 100 // 0.1ms : duration before next sound acquisition

// MACROS
//==============================
/**
** \brief Converts the analog input to a digital output
*/
#define ADC_CONVERT(average, adc_max_value) (((average) * (ADC_VREF)) / ((adc_max_value)-1)); // / (adc_max_value - 1);

//=============================================================================
// CONSTANTS
// ============================================================================
const uint8_t PINS_LED_BAR[NB_BAR_LEDS] = {PIN_BAR_1, PIN_BAR_2, PIN_BAR_3, PIN_BAR_4, PIN_BAR_5, PIN_BAR_6, PIN_BAR_7, PIN_BAR_8, PIN_BAR_9, PIN_BAR_10};

//=============================================================================
// VARIABLES
// ============================================================================
absolute_time_t timer_sound_sensor; // Timer for sound acquisition

uint16_t adc_values[ADC_ARRAY_LENGHT]{0};         // Stores the values read from the adc_read() function
uint16_t adc_average_values[ADC_ARRAY_LENGHT]{0}; // Stores the average values read from the adc_read() function

uint8_t current_adc_values_index = 0;
uint16_t adc_average = 0;

uint16_t adc_max_value = (1 << 8);

//=============================================================================
// FUNCTIONS
// ============================================================================
/**
** \brief Displays a roll effect on the bar graph
*/
void roll_bar_graph();

/**
** \brief Displays the sound intensity on the bar graph
*/
void display_sound_intensity();

/**
** \brief Sets the bar LEDs to OFF starting with the higher one
*/
void shutdown_led_bars();

#endif // SOUND_SENSOR_H