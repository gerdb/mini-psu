/*
 *  Project:      Mini_PSU
 *  File:         oled.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  OLED low level driver
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "oled.h"
#include "font_small.h"
#include "font_medium.h"
#include "font_large.h"
#include <math.h>

uint8_t scope_data[96];
int scope_index = 0;
/**
 * Initialize the OLED
 */
void oled_init(void) {

	volatile int i;

	GPIO_InitTypeDef GPIO_InitStructure;


	for (i = 0; i < 96; i++) {
		scope_data[i] = 8*(sin(((float)i)/6)+1);
	}

	// Enable GPIO clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	// SPI CS pin configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	OLED_VCC_OFF();

	//initialize the SPI
	spi_init();

	OLED_NORESET()
	;
	for (i = 0; i < 100; i++)
		;
	// reset the display
	OLED_RESET()
	;
	// Short delay
	for (i = 0; i < 1000; i++)
		;
	OLED_NORESET()
	;

	for (i = 0; i < 1000; i++)
		;
	// Initialization Sequence
	oled_write_command(SSD1331_CMD_DISPLAYOFF); // 0xAE
	oled_write_command(SSD1331_CMD_NORMALDISPLAY); // 0xA4
	oled_write_2commands(SSD1331_CMD_CLOCKDIV, 0xF0); // 0xB3 // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
	oled_write_2commands(SSD1331_CMD_DISPLAYOFFSET, 0x00); // 0xA2
	oled_write_2commands(SSD1331_CMD_SETMULTIPLEX, 0x3F); // 0xA8 // 0x3F 1/64 duty
	oled_write_2commands(SSD1331_CMD_SETMASTER, 0x8E); // 0xAD // (External VCC Supply Selected
	oled_write_2commands(SSD1331_CMD_STARTLINE, 0x00); // 0xA1
	oled_write_2commands(SSD1331_CMD_SETREMAP, 0x33); // 0xA0
	oled_write_2commands(SSD1331_CMD_MASTERCURRENT, 0x06); // 0x87
	oled_write_2commands(SSD1331_CMD_CONTRASTA, 0x91); // 0x81
	oled_write_2commands(SSD1331_CMD_CONTRASTB, 0x50); // 0x82
	oled_write_2commands(SSD1331_CMD_CONTRASTC, 0x7D); // 0x83
	oled_write_2commands(SSD1331_CMD_PRECHARGELEVEL, 0x3A); // 0xBB
	oled_write_2commands(SSD1331_CMD_PRECHARGEA, 0x64); // 0x8A
	oled_write_2commands(SSD1331_CMD_PRECHARGEB, 0x78); // 0x8B
	oled_write_2commands(SSD1331_CMD_PRECHARGEA, 0x64); // 0x8C
	oled_write_2commands(SSD1331_CMD_VCOMH, 0x3E); // 0xBE
	oled_write_2commands(SSD1331_CMD_PRECHARGE, 0x31); // 0xB1
	oled_write_2commands(SSD1331_CMD_POWERMODE, 0x0B); // 0xB0
	oled_write_command(SSD1331_CMD_CLEAR); // 0x25
	oled_write_command(0);
	oled_write_command(0);
	oled_write_command(OLED_WIDTH-1);
	oled_write_command(OLED_HEIGHT-1);


	OLED_VCC_ON();
	// delay 100ms
	for (i = 0; i < 1000000; i++)
		;

	oled_write_command(SSD1331_CMD_DISPLAYON);	//AF turn on oled panel
}

/**
 * Send a data byte
 */
void oled_write_data(uint8_t byte) {

	// Send a data byte
	OLED_DC_DATA()
	;

	//send the byte via SPI
	spi_send(byte);

}

/**
 * Send a command byte
 */
void oled_write_command(uint8_t byte) {

	// Send a command byte
	OLED_DC_COMMAND()
	;

	//send the byte via SPI
	spi_send(byte);

}
/**
 * Send a command byte
 */
void oled_write_2commands(uint8_t byte1, uint8_t byte2) {

	oled_write_command(byte1);
	oled_write_command(byte2);

}

void oled_goto(uint8_t x, uint8_t y, uint8_t h ) {
	if ((x >= OLED_WIDTH) || (y >= OLED_HEIGHT))
		return;

// set x and y coordinate
	oled_write_command(SSD1331_CMD_SETCOLUMN);
	oled_write_command(x);
	oled_write_command(OLED_WIDTH - 1);

	oled_write_command(SSD1331_CMD_SETROW);
	oled_write_command(y);
	h = y + h - 1;
	if (h > (OLED_HEIGHT-1))
		h = OLED_HEIGHT-1;
	oled_write_command(h);
}

void oled_writeCharLarge(char c, uint8_t color){

	int x, y,yy,yyy;
	int w, wo;

	uint8_t b;
	if (c == '.') {
		w = 6;
		wo = 6;
	}
	else {
		w = 18;
		wo = 0;
	}
	for (x=wo; x<(wo+w); x++) {
		for (y=0; y<4; y++) {
			b = font_large[c-32][x*4 + (3-y)];
			for (yy=0; yy<8; yy++) {
				yyy = y*8 + yy;
				if ((yyy >= 4) && (yyy <28)) {
					if ((b & 0x80) != 0)
						oled_write_data(color);
					else
						oled_write_data(OLED_BLACK);
				}
				b <<= 1;
			}

		}
	}

}
void oled_writeCharMedium(char c, uint8_t color){

	int x, y,yy;
	int w, wo;

	uint8_t b;
	if (c == '.') {
		w = 8;
		wo = 2;
	}
	else {
		w = 12;
		wo = 0;
	}
	for (x=wo; x<(wo+w); x++) {
		for (y=0; y<2; y++) {
			b = font_medium[c-32][x*2 + (1-y)];
			for (yy=0; yy<8; yy++) {
				if ((b & 0x80) != 0)
					oled_write_data(color);
				else
					oled_write_data(OLED_BLACK);
				b <<= 1;
			}

		}
	}

}
void oled_writeCharSmall(char c, uint8_t color){

	int x, yy;

	uint8_t b;

	for (x=0; x<5; x++) {
		b = font_small[c*5+x];
		for (yy=0; yy<8; yy++) {
			if ((b & 0x80) != 0)
				oled_write_data(color);
			else
				oled_write_data(OLED_BLACK);
			b <<= 1;
		}
	}

}

void oled_writeStringLarge(uint8_t x, uint8_t y, char s[], uint8_t color){

	int n=0;
	oled_goto (x, y, 24);

	while (s[n]) {
		oled_writeCharLarge(s[n], color);
		n++;
	}
}

void oled_writeStringMedium(uint8_t x, uint8_t y, char s[], uint8_t color){

	int n=0;
	oled_goto (x, y, 16);

	while (s[n]) {
		oled_writeCharMedium(s[n], color);
		n++;
	}
}

void oled_writeStringSmall(uint8_t x, uint8_t y, char s[], uint8_t color){

	int n=0;
	oled_goto (x, y, 8);

	while (s[n]) {
		oled_writeCharSmall(s[n], color);
		n++;
	}
}

/*
 * Draw a scope to display the output current
 */
void oled_drawScope (int pnewval){
	int x, y, n;
	int val;

	// Limit the input value to 0..15
	int newval  = pnewval;
	if (newval>15)
		newval = 15;
	if (newval<0)
		newval = 0;

	oled_goto (0, 48, 16);
	n = scope_index;
	for (x = 0; x < 96 ; x++) {

		if (x == 95) {
			scope_data[n] = newval;
		}
		val = scope_data[n];
		for (y = 0; y < 16; y++) {
			if (y == (15-val))
				oled_write_data(OLED_YELLOW);
			else
				oled_write_data(OLED_BLACK);
		}

		n++;
		if (n >= 96)
			n = 0;
	}
	scope_index++;
	if (scope_index >= 96)
		scope_index = 0;
}

/*
 * Clear the screen
 */
void oled_clear(void){
	int x, y;

	oled_goto (0, 0, 64);
	for (y = 0; y < 64 ; y++) {
		for (x = 0; x < 96 ; x++) {
			oled_write_data(OLED_BLACK);
		}
	}

}
