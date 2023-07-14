/**
 * @file screens.h
 * @author Aeldit (raphael.roger.92@gmail.com)
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
void screen_temp_hum(GFX *display, float temperature, float humidity);
void screen_debug(GFX *display, uint8_t selector);
void screen_option_toggle(GFX *display, char *text, bool option, uint8_t selected_option);
