/**
 * @file screens.cpp
 * @author Raphoulfifou (raphael.roger.92@gmail.com)
 */

//=============================================================
// INCLUDES
// ============================================================
#include "screens.h"

//=============================================================
// DEFINES
// ============================================================
#define FIRST_LINE_Y 15
#define TEXT_OPTION_POS 54

//=============================================================
// FUNCTIONS
// ============================================================
void draw_title(GFX *display, char *text);
void draw_centered_title(GFX *display, char *text, bool back_button_selected);
void draw_text_reverse(GFX *display, int x, int y, char *text);

// Screens
//=======================================================
void screen_main_menu(GFX *display, uint8_t selector)
{
    char Text[20];

    display->clear();

    // Screen header
    sprintf(Text, "Main Menu");
    draw_centered_title(display, Text, false);

    // Menu 1
    sprintf(Text, "Options");
    display->drawString(10, FIRST_LINE_Y, Text);

    // Menu 2
    sprintf(Text, "Debug");
    display->drawString(10, FIRST_LINE_Y + 10, "Debug");

    // Selected menu
    sprintf(Text, ">");
    display->drawString(0, FIRST_LINE_Y + (selector)*10, Text);

    display->display();
}

void screen_options(GFX *display, uint8_t selector, t_options *options, uint8_t arraySize)
{
    char Text[20];

    display->clear();

    // Screen header
    sprintf(Text, "Options");

    if (selector == -1)
    {
        draw_centered_title(display, Text, true);
    }
    else
    {
        draw_centered_title(display, Text, false);
    }

    for (int i = 0; i < arraySize; i++)
    {
        sprintf(Text, "%s", options->options_names[i]);
        display->drawString(0, FIRST_LINE_Y + 10 * i, Text);

        if (options->values[i])
        {
            sprintf(Text, "%c", C_CAR_CHECKED);
        }
        else
        {
            sprintf(Text, "%c", C_CAR_UNCHECKED);
        }
        display->drawString(122, FIRST_LINE_Y + 10 * i, Text);
    }

    // Menu 1
    /*sprintf(Text, "Life LED");
    display->drawString(10, FIRST_LINE_Y, Text);

    // Menu 2
    sprintf(Text, "Screen Timeout");
    display->drawString(10, FIRST_LINE_Y + 10, Text);

    // Selected menu
    sprintf(Text, "-");
    display->drawString(0, FIRST_LINE_Y + (selector)*10, Text);

    // Options values
    sprintf(Text, "%c", C_CAR_CHECKED);
    display->drawString(0, FIRST_LINE_Y, Text);*/

    display->display();
}

void screen_temp_hum(GFX *display, float temperature, float humidity)
{
    char Text[20];

    display->clear();
    // Menu
    sprintf(Text, "Temperature");
    draw_title(display, Text);

    // Temperature
    sprintf(Text, "Temperature : %.1fC", temperature);
    display->drawString(0, FIRST_LINE_Y, Text);

    // Humidity
    sprintf(Text, "Humidity : %.1f%%", humidity);
    display->drawString(0, FIRST_LINE_Y + 10, Text);

    display->display();
}

void screen_debug(GFX *display, uint8_t selector)
{
    char Text[20];

    display->clear();
    // Menu
    sprintf(Text, "DEBUG");
    draw_title(display, Text);

    // Life LED
    sprintf(Text, "Life LED");
    display->drawString(10, FIRST_LINE_Y, Text);

    // Selected menu
    sprintf(Text, ">");
    display->drawString(0, FIRST_LINE_Y + (selector)*10, Text);

    display->display();
}

void screen_option_toggle(GFX *display, char *text, bool option, uint8_t selected_option)
{
    char Text[20];

    display->clear();

    // Title
    sprintf(Text, "Toogle:%s", text);
    draw_title(display, Text);

    // Current state of the option
    if (option)
    {
        sprintf(Text, "Current state : %c", C_CAR_CHECKED);
        display->drawString(0, FIRST_LINE_Y, Text);
    }
    else
    {
        sprintf(Text, "Current state : %c", C_CAR_UNCHECKED);
        display->drawString(0, FIRST_LINE_Y, Text);
    }

    if (selected_option == 0)
    {
        sprintf(Text, "OK");
        draw_text_reverse(display, 1, TEXT_OPTION_POS, Text);
        sprintf(Text, "CANCEL");
        display->drawString(92, TEXT_OPTION_POS, Text);
    }
    else if (selected_option == 1)
    {
        sprintf(Text, "OK");
        display->drawString(1, TEXT_OPTION_POS, Text);
        sprintf(Text, "CANCEL");
        draw_text_reverse(display, 92, TEXT_OPTION_POS, Text);
    }

    display->display();
}

// Utils
//=======================================================
void draw_title(GFX *display, char *text)
{
    display->drawFillRectangle(0, 0, 128, 10, colors::WHITE);
    display->drawString(1, 1, text, colors::BLACK);
}

void draw_centered_title(GFX *display, char *text, bool back_button_selected)
{
    display->drawFillRectangle(0, 0, 128, 10, colors::WHITE);

    if (back_button_selected)
    {
        display->drawFillRectangle(0, 0, 9, 10, colors::BLACK);
        display->drawString(0, 1, "<", colors::WHITE);
    }
    else
    {
        display->drawString(0, 1, "<", colors::BLACK);
    }
    display->drawString(64 + sizeof(text) - 1 - sizeof(text) * 6, 1, text, colors::BLACK);
}

void draw_text_reverse(GFX *display, int x, int y, char *text)
{
    if ((x > 0) && (y > 0))
    {
        display->drawFillRectangle(x - 1, y - 1, strlen(text) * (font_8x5[1] + 1) + 1, 10, colors::WHITE);
        display->drawString(x, y, text, colors::BLACK);
    }
}
