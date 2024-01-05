/**
 * \author Aeldit
 * \link https://github.com/Aeldit
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

#define PREV_VALUES_MAX_NB 128

//=============================================================
// FUNCTIONS
// ============================================================
void screen_main_menu(GFX *display, uint8_t only_update, uint16_t frequence);

/**
** \param display A pointer to the GFX object
** \param prev_values The 63 previous values of the frequence
*/
void display_graph_screen(GFX *display, uint16_t prev_values[PREV_VALUES_MAX_NB]);

/**
** \brief Draws a white line of width 128 and of height 10 at the top of the
**        screen + writes the given text on it (centered)
**
** \param display A pointer to the GFX object
** \param text The screen header title
** \param back_button_selected Whether the "back" button is selected
*/
void draw_centered_header(GFX *display, char text[MAX_CHAR_PER_LINE]);

#endif
