/**
 * @file screens.h
 * @author Raphoulfifou (raphael.roger.92@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-08-25
 *
 * @copyright Copyright (c) 2022
 *
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
void screen_option_toogle(GFX *display, char *text, bool option, uint8_t selected_option);
