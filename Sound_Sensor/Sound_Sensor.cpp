/**
 * @author Aeldit
 * @link https://github.com/Aeldit
 */

#include "hardware/i2c.h"

#include "sound_sensor.hpp"
#include "GFX.hpp"
#include "logo.hpp"
#include "screens.h"

int main()
{
    stdio_init_all();

    // ADC
    adc_init();
    adc_gpio_init(PIN_ANALOG_INPUT);
    adc_select_input(ADC_NUM);

    // LCD
    //==============================
    i2c_init(i2c1, 400000);                         // Initialize I2C on i2c0 port with 400kHz
    gpio_set_function(PIN_DISP_SDA, GPIO_FUNC_I2C); // Use PIN_DISP_SDA as I2C
    gpio_set_function(PIN_DISP_SCL, GPIO_FUNC_I2C); // Use PIN_DISP_SCL as I2C
    gpio_pull_up(PIN_DISP_SDA);                     // Pull up PIN_DISP_SDA
    gpio_pull_up(PIN_DISP_SCL);                     // Pull up PIN_DISP_SCL

    GFX *oled = new GFX(0x3C, size::W128xH64, i2c1); // Declare oled instance
    oled->display(logo);

    // Bar LEDs
    for (int i = 0; i < NB_BAR_LEDS; i++)
    {
        gpio_init(PINS_LED_BAR[i]);
        gpio_set_dir(PINS_LED_BAR[i], GPIO_OUT);
    }

    roll_bar_graph();

    timer_sound_sensor = timer_lcd = get_absolute_time();
    screen_main_menu(oled, 0, adc_average);
    uint8_t display_update_needed = 1;
    uint16_t frequence_to_display;

    uint16_t prev_frequences[PREV_VALUES_MAX_NB] = {0};
    uint8_t prev_frequences_idx = 0;

    while (1)
    {
        if (absolute_time_diff_us(timer_sound_sensor, get_absolute_time()) > C_TIME_SOUND_ACQUISITION)
        {
            adc_values[current_adc_values_index++] = adc_read();

            // If the array has been filled
            if (current_adc_values_index == ADC_ARRAY_LENGHT)
            {
                current_adc_values_index = 0;

                // Calculs the average value of the signal
                // 2**7 * 2**12 = 2**19 => uint16_t bits is too small, so we use
                // uint32_t
                uint32_t adc_average_sum = 0;

                for (int i = 0; i < ADC_ARRAY_LENGHT; i++)
                {
                    adc_average_sum += adc_values[i];
                }
                adc_average = adc_average_sum / ADC_ARRAY_LENGHT;

                // Brings the average value back to 0 and calculates the average
                // values from there
                adc_average_sum = 0;
                uint16_t tmp_max_value = 0;

                for (int i = 0; i < ADC_ARRAY_LENGHT; i++)
                {
                    if (adc_values[i] > adc_average)
                    {
                        adc_average_sum += adc_values[i] - adc_average;
                    }
                    else
                    {
                        adc_average_sum += adc_average - adc_values[i];
                    }

                    if (adc_values[i] > tmp_max_value)
                    {
                        tmp_max_value = adc_values[i];
                    }
                }
                adc_average = adc_average_sum / ADC_ARRAY_LENGHT;
                adc_max_value = tmp_max_value;
                frequence_to_display = adc_average;

                // Used to display the graph on the LCD
                if (prev_frequences_idx < PREV_VALUES_MAX_NB)
                {
                    prev_frequences[prev_frequences_idx++] = adc_average;
                }
                else
                {
                    for (int i = 0; i < PREV_VALUES_MAX_NB - 2; i++)
                    {
                        prev_frequences[i] = prev_frequences[i + 1];
                    }
                    prev_frequences[PREV_VALUES_MAX_NB - 1] = adc_average;
                }

                uint8_t tmp_adc_converted =
                    ADC_CONVERT(adc_average, adc_max_value);

                if (tmp_adc_converted > adc_max_value)
                {
                    adc_max_value = tmp_adc_converted;
                }
            }

            display_sound_intensity();

            timer_sound_sensor = get_absolute_time();
            display_update_needed = 1;
        }

        if (display_update_needed && absolute_time_diff_us(timer_lcd, get_absolute_time()) > C_TIME_LCD_REFRESH)
        {
            display_graph_screen(oled, prev_frequences);

            display_update_needed = 0;
            timer_lcd = get_absolute_time();
        }
    }
}

void roll_bar_graph()
{
    for (int i = 0; i < 10; i++)
    {
        gpio_put(PINS_LED_BAR[i], HIGH);
        sleep_ms(100);
        gpio_put(PINS_LED_BAR[i], LOW);
    }
}

void display_sound_intensity()
{
    shutdown_led_bars();
    for (int i = 0; i < adc_average; i++)
    {
        gpio_put(PINS_LED_BAR[i], HIGH);
    }
}

void shutdown_led_bars()
{
    for (int i = NB_BAR_LEDS; i >= 0; i--)
    {
        if (adc_average < i)
        {
            gpio_put(PINS_LED_BAR[i], LOW);
        }
    }
}