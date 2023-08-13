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
                        GP2  | 4            37 |  3V3_EN
                        GP3  | 5            36 |  3V3_OUT
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
                        GP12 | 16           25 |  GP19     ECHO
                        GP13 | 17           24 |  GP18     TRIG
                        GND <| 18           23 |> GND
                        GP14 | 19           22 |  GP17
                        GP15 | 20           21 |  GP16
                             +-----------------+
*/
//===============================================================================

//=============================================================
// INCLUDES
// ============================================================
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/sync.h"

//=============================================================
// DEFINES
// ============================================================
#define LOW 0
#define HIGH 1

#define EDGE_FALL 4
#define EDGE_RISE 8

// PINS
//==============================
#define PIN_TRIG 18 // PICO Output & HC-SR04 Input
#define PIN_ECHO 19 // PICO Input & HC-SR04 Output

// Tempos
//==============================
#define C_TIME_HCSR04_ACQUISITION 1000 * 1000 // 1s : duration before next acquisition

//=============================================================
// VARIABLES
// ============================================================
absolute_time_t timer_HCSR04; // Timer for the HCSR04 sensor's next acquisition

absolute_time_t HCSR04_signal;
int64_t time_echo;

float distance; // Distance mesured by the HC-SR04 sensor

bool time_changed = false;

//=============================================================
// FUNCTIONS
// ============================================================
void gpio_callback(uint gpio, uint32_t events);

int main()
{
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    gpio_init(PIN_TRIG);
    gpio_set_dir(PIN_TRIG, GPIO_OUT);

    gpio_init(PIN_ECHO);
    gpio_set_dir(PIN_ECHO, GPIO_IN);

    gpio_set_irq_enabled_with_callback(PIN_ECHO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_put(25, HIGH);

    timer_HCSR04 = get_absolute_time();

    while (true)
    {
        if (absolute_time_diff_us(timer_HCSR04, get_absolute_time()) > C_TIME_HCSR04_ACQUISITION)
        {
            gpio_put(PIN_TRIG, HIGH);
            sleep_us(10);
            gpio_put(PIN_TRIG, LOW);
            timer_HCSR04 = get_absolute_time();
        }

        if (time_changed)
        {
            uint32_t saved_interruption = save_and_disable_interrupts();
            distance = ((float)time_echo * 340.0 / 2.0) / 10000.0;
            printf("%d\n", time_echo);
            printf("Distance (cm) : %f\n", distance);
            time_changed = false;
            restore_interrupts(saved_interruption);
        }
    }
}

void gpio_callback(uint gpio_pin, uint32_t events)
{
    if (events == GPIO_IRQ_EDGE_RISE)
    {
        HCSR04_signal = get_absolute_time();
    }
    else if (events == GPIO_IRQ_EDGE_FALL)
    {
        time_echo = absolute_time_diff_us(HCSR04_signal, get_absolute_time());
        time_changed = true;
    }
}
