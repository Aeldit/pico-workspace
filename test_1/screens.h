/**
 * @author Aeldit
 * @link https://github.com/Aeldit
 */

//=============================================================
// INCLUDES
// ============================================================
#include <GFX.hpp>
#include "types.h"

//=============================================================
// FUNCTIONS
// ============================================================
void screen_main_menu(GFX *display, uint8_t selector);
void screen_options(GFX *display, int selector, t_options *options, uint8_t arraySize);
void screen_temp_hum(GFX *display, float temperature, float humidity, bool reload_only_values);
