/**
 * DHT22-Pico v0.2.0 by Eleanor McMurtry (2021)
 * Loosely based on code by Caroline Dunn: https://github.com/carolinedunn/pico-weather-station
 **/

#include "dht22_pico.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <math.h>

#define cTimeHostToStartms 10      // Tbe : ms (min: 0.8ms; Typ: 1ms; max: 20ms)
#define cTimeBusReleasedus 40      // Tgo : us (min: 20us; Typ: 30us; max: 200us)
#define cTimeResponseLHus 100      // Trel, Treh : us (min: 75us; Typ: 80us; max: 85us)
#define cTimeSignalLowus 200       // Tlow : us (min: 48us; Typ: 50us; max: 55us)
#define cTimeSignal0Highus 50      // Th0 : us (min: 22us; Typ: 26us; max: 30us)
#define cTimeSignal1Highus 100     // Th1 : us (min: 68us; Typ: 70us; max: 75us)
#define cTimeSensorReleaseBusus 70 // Th1 : us (min: 45us; Typ: 50us; max: 55us)
#define cTimePrecisionus 10        // us (sleep duration for status change waiting)

#define cTimeOutDHTus 500

dht_reading dht_init(uint8_t pin)
{
    gpio_init(pin);
    dht_reading dht;
    dht.pin = pin;
    return dht;
}

static uint32_t wait_for(uint8_t pin, uint8_t expect, uint32_t tmax)
{
    uint32_t then = time_us_32();
    while ((expect != gpio_get(pin)) && ((time_us_32() - then) < tmax))
    {
        sleep_us(cTimePrecisionus);
    }
    if ((time_us_32() - then) < tmax)
        return (time_us_32() - then);
    else
        return tmax;
}

inline static float word(uint8_t first, uint8_t second)
{
    return (float)((first << 8) + second);
}

// helper function to read from the DHT
// more: https://cdn-shop.adafruit.com/datasheets/Digital+humidity+and+temperature+sensor+AM2302.pdf
uint8_t dht_read(dht_reading *dht)
{
    uint8_t data[5] = {0, 0, 0, 0, 0};

    // request a sample
    gpio_set_dir(dht->pin, GPIO_OUT);
    gpio_put(dht->pin, 0);
    sleep_ms(cTimeHostToStartms);
    gpio_put(dht->pin, 1);
    sleep_us(cTimeBusReleasedus);

    // wait for acknowledgement
    gpio_set_dir(dht->pin, GPIO_IN);
    if (wait_for(dht->pin, 0, cTimeResponseLHus) == cTimeResponseLHus)
        return DHT_NO_SENSOR;
    if (wait_for(dht->pin, 1, cTimeResponseLHus) == cTimeResponseLHus)
        return DHT_NO_SENSOR;
    // if (wait_for(dht->pin, 0, cTimeResponseLHus) == cTimeResponseLHus)
    if (wait_for(dht->pin, 0, 200) == 200)
        return DHT_NO_SENSOR;

    // read sample (40 bits = 5 bytes)
    for (uint8_t bit = 0; bit < 40; ++bit)
    {
        if (wait_for(dht->pin, 1, cTimeSignalLowus) == cTimeSignalLowus)
            return DHT_NO_SENSOR;
        uint8_t count = wait_for(dht->pin, 0, cTimeResponseLHus);
        if (count == cTimeResponseLHus)
            return DHT_NO_SENSOR;
        data[bit / 8] <<= 1;
        if (count > cTimeSignal0Highus)
        {
            data[bit / 8] |= 1;
        }
    }

    // pull back up to mark end of read
    gpio_set_dir(dht->pin, GPIO_OUT);
    gpio_put(dht->pin, 1);

    // checksum
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
    {
        float humidity = word(data[0], data[1]) / 10;
        float temp = word(data[2] & 0x7F, data[3]) / 10;

        // if the highest bit is 1, temperature is negative
        if (data[2] & 0x80)
        {
            temp = -temp;
        }

        // check if checksum was OK but something else went wrong
        if (isnan(temp) || isnan(humidity) || temp == 0)
        {
            return DHT_ERR_NAN;
        }
        else
        {
            dht->humidity = humidity;
            dht->temp_celsius = temp;
            return DHT_OK;
        }
    }
    else
    {
        return DHT_ERR_CHECKSUM;
    }
}
