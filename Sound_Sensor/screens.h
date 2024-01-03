/**
 * @author Aeldit
 * @link https://github.com/Aeldit
 */

#ifndef SCREENS_H
#define SCREENS_H
//=============================================================
// INCLUDES
// ============================================================
#include <GFX.hpp>

#include <string.h>

//=============================================================
// DEFINES
// ============================================================
#define FIRST_LINE_Y 15

#define MAX_CHAR_PER_LINE 19

//=============================================================
// FUNCTIONS
// ============================================================
void screen_main_menu(GFX *display, uint8_t only_update, uint16_t frequence);

/**
 * @brief Draws a white rectangle ate the top of the screen and write on it the text (centered).
 *        It also draws the back button
 *
 * @param display The GFX instance
 * @param text The screen header title
 * @param back_button_selected Whether the "back" button is selected
 */
void draw_centered_header(GFX *display, char *text);

void draw_text_reverse(GFX *display, int x, int y, char *text);

#endif
