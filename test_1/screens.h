/**
 * @file screens.h
 * @author Aeldit (raphael.roger.92@gmail.com)
 */

//=============================================================
// INCLUDES
// ============================================================
#include <GFX.hpp>

//=============================================================
// FUNCTIONS
// ============================================================
void screen_main_menu(GFX *display, uint8_t select);
void screen_temp_hum(GFX *display, float temperature, float humidity);
void screen_debug(GFX *display, uint8_t select);
void screen_option_toggle(GFX *display, char *text, bool option, uint8_t selected_option);
