#pragma once

#include "hardware/i2c.h"
#include "string.h"
#include "stdint.h"

#define SSD1306_I2C_ADDR 0x3C

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETPAGESTART 0xB0
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_NOP 0xE3
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

#define SSD1306_SCROLLSPEED_5 0x00
#define SSD1306_SCROLLSPEED_64 0x01
#define SSD1306_SCROLLSPEED_128 0x02
#define SSD1306_SCROLLSPEED_256 0x03
#define SSD1306_SCROLLSPEED_3 0x04
#define SSD1306_SCROLLSPEED_4 0x05
#define SSD1306_SCROLLSPEED_25 0x06
#define SSD1306_SCROLLSPEED_2 0x07

enum class colors
{
	BLACK,
	WHITE,
	INVERSE
};

enum class size
{
	W128xH64,
	W128xH32
};

class SSD1306
{
protected:
	uint16_t DevAddr;
	i2c_inst_t *i2c;
	uint8_t width;
	uint8_t height;
	size Size;

	unsigned char *buffer;

	void sendData(uint8_t *buffer, size_t buff_size);
	void sendCommand(uint8_t command);

public:
	SSD1306(uint16_t const DevAddr, size Size, i2c_inst_t *i2c);
	~SSD1306();

	void displayON(uint8_t On);
	void invertColors(uint8_t Invert);
	void rotateDisplay(uint8_t Rotate);
	void setContrast(uint8_t Contrast);
	void drawPixel(int16_t x, int16_t y, colors Color = colors::WHITE);
	void clear(colors Color = colors::BLACK);
	void display(unsigned char *data = nullptr);
	void horizontal_scroll(bool right, uint8_t start, uint8_t stop, uint8_t speed);
	void vertical_horizontal_scroll(bool right, uint8_t start, uint8_t stop, uint8_t speed, uint8_t vert);
	void stop_scroll();
	uint8_t getHeight();
	uint8_t getWidth();
};