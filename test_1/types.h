/**
 * @brief Custom types
 */

#ifndef __TEST_TYPES__
#define __TEST_TYPES__

// DHT
//==============================
#define C_ERROR_DHT -60 // Temperature sensor error value

typedef float temperature_t;
typedef float humidity_t;

// Options
//==============================
typedef char t_options_names[2][18];

typedef struct
{
    t_options_names options_names;
    bool values[2];
} t_options;

#endif
