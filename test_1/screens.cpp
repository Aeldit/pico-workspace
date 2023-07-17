/**
 * @author Aeldit
 * @link https://github.com/Aeldit
 */

//=============================================================
// INCLUDES
// ============================================================
#include "screens.h"

//=============================================================
// DEFINES
// ============================================================
#define FIRST_LINE_Y 15

//=============================================================
// FUNCTIONS
// ============================================================
/**
 * @brief Draws a white rectangle ate the top of the screen and write on it the text (centered).
 *        It also draws the back button
 *
 * @param display The GFX instance
 * @param text The screen header title
 * @param back_button_selected Whether the "back" button is selected
 */
void draw_centered_header(GFX *display, char *text, uint8_t length);
void draw_text_reverse(GFX *display, int x, int y, char *text);

// Screens
//=======================================================
void screen_main_menu(GFX *display, uint8_t selector)
{
    char Text[19];
    display->clear();

    // Screen header
    sprintf(Text, "Main Menu");
    draw_centered_header(display, Text, 9);

    sprintf(Text, "Options");
    display->drawString(8, FIRST_LINE_Y, Text);

    sprintf(Text, "Temperature");
    display->drawString(8, FIRST_LINE_Y + 10, Text);

    // Selected menu
    sprintf(Text, ">");
    display->drawString(0, FIRST_LINE_Y + (selector)*10, Text);

    display->display();
}

void screen_options(GFX *display, int selector, t_options *options, uint8_t arraySize)
{
    char Text[19];
    display->clear();

    // Screen header
    sprintf(Text, "Options");
    draw_centered_header(display, Text, 7);

    // Screen body
    for (int i = 0; i < arraySize; i++)
    {
        sprintf(Text, "%s", options->names[i]);
        display->drawString(0, FIRST_LINE_Y + 10 * i, Text);

        // Display a ✓ or a X depending on the value of the boolean option
        if (options->values[i])
        {
            sprintf(Text, "%c", C_CAR_CHECKED);
        }
        else
        {
            sprintf(Text, "%c", C_CAR_UNCHECKED);
        }

        if (selector == i)
        {
            display->drawFillRectangle(121, FIRST_LINE_Y + 10 * i, 7, 10, colors::WHITE);
            display->drawString(122, FIRST_LINE_Y + 10 * i, Text, colors::BLACK);
        }
        else
        {
            display->drawString(122, FIRST_LINE_Y + 10 * i, Text, colors::WHITE);
        }
    }
    display->display();
}

void screen_temp_hum(GFX *display, t_temperature temperature, t_humidity humidity, bool reload_only_values)
{
    char Text[20];

    if (!reload_only_values)
    {
        display->clear();

        // Screen header
        sprintf(Text, "Temperature");
        draw_centered_header(display, Text, 11);

        // Temperature
        sprintf(Text, "Temperature :", temperature);
        display->drawString(0, FIRST_LINE_Y, Text);

        // Humidity
        sprintf(Text, "Humidity :", humidity);
        display->drawString(0, FIRST_LINE_Y + 10, Text);
    }

    // Temperature
    sprintf(Text, "%.1fC", temperature);
    display->drawString(98, FIRST_LINE_Y, Text);

    // Humidity
    sprintf(Text, "%.1f%%", humidity);
    display->drawString(98, FIRST_LINE_Y + 10, Text);

    display->display();
}

// Utils
//=======================================================

void draw_centered_header(GFX *display, char *text, uint8_t length)
{
    display->drawFillRectangle(0, 0, 128, 10, colors::WHITE);

    if (length % 2 == 0)
    {
        display->drawString(64 - (length / 2) * 5 - length + 3, 1, text, colors::BLACK);
    }
    else
    {
        display->drawString(64 - (length / 2) * 5 - length + 6, 1, text, colors::BLACK);
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
