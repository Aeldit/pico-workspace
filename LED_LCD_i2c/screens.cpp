/**
 * @file screens.cpp
 * @author Raphoulfifou (raphael.roger.92@gmail.com)
 * @brief Screens for LED_LCD_i2c.cpp LCD
 * @version 0.1
 * @date 2022-08-25
 *
 * @copyright Copyright (c) 2022
 *
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
void draw_text_reverse(GFX *display, int x, int y, char *text);

// Screens
//=======================================================
void screen_main_menu(GFX *display, uint8_t select)
{
    char Text[20];

    display->clear();
    // Menu
    sprintf(Text, "Main Menu");
    draw_title(display, Text);

    // Menu 1
    sprintf(Text, "Temperature");
    display->drawString(10, FIRST_LINE_Y, Text);

    // Menu 2
    sprintf(Text, "Debug");
    display->drawString(10, FIRST_LINE_Y + 10, Text);

    // Selected menu
    sprintf(Text, ">");
    display->drawString(0, FIRST_LINE_Y + (select)*10, Text);

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

void screen_debug(GFX *display, uint8_t select)
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
    display->drawString(0, FIRST_LINE_Y + (select)*10, Text);

    display->display();
}

void screen_option_toogle(GFX *display, char *text, bool option, uint8_t selected_option)
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

    /*if (selected_option == 0)
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
    }*/

    display->display();
}

// Utils
//=======================================================
void draw_title(GFX *display, char *text)
{
    display->drawFillRectangle(0, 0, 128, 10, colors::WHITE);
    display->drawString(1, 1, text, colors::BLACK);
}

void draw_text_reverse(GFX *display, int x, int y, char *text)
{
    if ((x > 0) && (y > 0))
    {
        display->drawFillRectangle(x - 1, y - 1, strlen(text) * (font_8x5[1] + 1) + 1, 10, colors::WHITE);
        display->drawString(x, y, text, colors::BLACK);
    }
}
