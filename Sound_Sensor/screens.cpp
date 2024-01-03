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

// Utils
//=======================================================

void draw_centered_header(GFX *display, char *text)
{
    uint8_t len = strlen(text);
    display->drawFillRectangle(0, 0, 128, 10, colors::WHITE);

    if (len % 2 == 0)
    {
        display->drawString(64 - (len / 2) * 5 - len + 3, 1, text, colors::BLACK);
    }
    else
    {
        display->drawString(64 - (len / 2) * 5 - len + 6, 1, text, colors::BLACK);
    }
}

void draw_text_reverse(GFX *display, int x, int y, char *text)
{
    if ((x > 0) && (y > 0))
    {
        display->drawFillRectangle(x - 1, y - 1, strlen(text) * (font_8x5[1] + 1) + 1, 10, colors::WHITE);
        display->drawString(x, y, text, colors::BLACK);
    }
}
