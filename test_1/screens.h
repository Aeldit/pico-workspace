/**
 * @author Aeldit
 * @link https://github.com/Aeldit
 */

//=============================================================
// INCLUDES
// ============================================================
#include "types.h"
#include <GFX.hpp>

//=============================================================
// FUNCTIONS
// ============================================================
void screen_main_menu(GFX *display, uint8_t selector);
void screen_options(GFX *display, int selector, t_options *options, uint8_t arraySize);
void screen_temp_hum(GFX *display, t_temperature temperature, t_humidity humidity, bool reload_only_values);
