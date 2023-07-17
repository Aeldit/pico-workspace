/**
 * @brief Custom types
 */

#ifndef __TEST_TYPES__
#define __TEST_TYPES__

// DHT
//==============================
#define C_ERROR_DHT -60 // Temperature sensor error value

typedef float t_temperature;
typedef float t_humidity;

// Options
//==============================
#define NB_OPTIONS 3

typedef char t_options_names[NB_OPTIONS][18];

typedef struct
{
    t_options_names names;
    bool values[NB_OPTIONS];
} t_options;

#endif
