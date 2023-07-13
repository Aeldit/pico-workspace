/**
 * @brief Custom types
 */

#ifndef __TEST_TYPES__
#define __TEST_TYPES__

typedef char t_options_names[2][18];

typedef struct
{
    t_options_names options_names;
    bool values[2];
} t_options;

#endif
