/**
 * @author Aeldit
 * @link https://github.com/Aeldit
 */

//=============================================================
// INCLUDES
// ============================================================
#include "screens.h"

//=============================================================
// FUNCTIONS
// ============================================================
// Screens
//=======================================================
void screen_main_menu(GFX *display, uint8_t only_update, uint16_t frequence)
{
    if (only_update)
    {
        // 79 = 8 + strlen("Frequence : ") | 23 = 4 * 5 + 3
        display->drawFillRectangle(79, FIRST_LINE_Y, 23, 8, colors::BLACK);

        char Text[3];
        sprintf(Text, "%u", frequence);
        display->drawString(79, FIRST_LINE_Y, Text);
    }
    else
    {
        char Text[MAX_CHAR_PER_LINE];
        display->clear();

        // Screen header
        sprintf(Text, "Main Menu");
        draw_centered_header(display, Text);

        sprintf(Text, "Frequence : %u", frequence);
        display->drawString(8, FIRST_LINE_Y, Text);
    }
    display->display();
}

void display_graph_screen(GFX *display, uint16_t prev_values[PREV_VALUES_MAX_NB])
{
    char Text[MAX_CHAR_PER_LINE];
    display->clear();

    // Screen header
    sprintf(Text, "Graph");
    draw_centered_header(display, Text);

    for (int i = 0; i < PREV_VALUES_MAX_NB; i++)
    {
        // 37 = 10 (header) + 27 (middle of the screen)
        display->drawPixel(i, 37 + prev_values[i], colors::WHITE);
    }

    display->display();
}

// Utils
//=======================================================
void draw_centered_header(GFX *display, char text[MAX_CHAR_PER_LINE])
{
    size_t len = strlen(text);
    display->drawFillRectangle(0, 0, 128, 10, colors::WHITE);

    display->drawString(64 - (len * 5 + len - 1) / 2, 1, text, colors::BLACK);
}