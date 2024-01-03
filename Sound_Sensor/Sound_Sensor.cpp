/**
 * @author Aeldit
 * @link https://github.com/Aeldit
 */

#include "sound_sensor.hpp"

int main()
{
    stdio_init_all();

    // ADC
    adc_init();
    adc_gpio_init(PIN_ANALOG_INPUT);
    adc_select_input(ADC_NUM);

    // Bar LEDs
    for (int i = 0; i < NB_BAR_LEDS; i++)
    {
        gpio_init(PINS_LED_BAR[i]);
        gpio_set_dir(PINS_LED_BAR[i], GPIO_OUT);
    }

    roll_bar_graph();

    timer_sound_sensor = get_absolute_time();

    while (true)
    {
        if (absolute_time_diff_us(timer_sound_sensor, get_absolute_time())
            > C_TIME_SOUND_ACQUISITION)
        {
            // to test :
            // adc_values[current_adc_values_index++] = adc_read();
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

                uint8_t tmp_adc_converted =
                    ADC_CONVERT(adc_average, adc_max_value);

                if (tmp_adc_converted > adc_max_value)
                {
                    adc_max_value = tmp_adc_converted;
                }
            }

            display_sound_intensity();

            timer_sound_sensor = get_absolute_time();
        }
    }
}

void roll_bar_graph()
{
    for (int i = 0; i < 10; i++)
    {
        gpio_put(PINS_LED_BAR[i], HIGH);
        sleep_ms(200);
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