/*
 *  Project:      Mini_PSU
 *  File:         oled.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for oled.c
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
#ifndef OLED_H_
#define OLED_H_

#include "spi.h"

// Function Prototypes
void oled_init(void);
void oled_write_data( uint8_t byte );
void oled_write_command( uint8_t byte );
void oled_write_2commands( uint8_t byte1 ,uint8_t byte2);
void oled_goto(uint8_t x, uint8_t y, uint8_t h );
void oled_writeStringSmall(uint8_t x, uint8_t y, char s[], uint8_t color);
void oled_writeStringMedium(uint8_t x, uint8_t y, char s[], uint8_t color);
void oled_writeStringLarge(uint8_t x, uint8_t y, char s[], uint8_t color);
void oled_writeCharSmall(char c, uint8_t color);
void oled_writeCharMedium(char c, uint8_t color);
void oled_writeCharLarge(char c, uint8_t color);
void oled_drawScope(int newval);
void oled_clear(void);

// Makros
#define OLED_DC_DATA()    GPIOC->BSRRL = GPIO_Pin_1; // High
#define OLED_DC_COMMAND() GPIOC->BSRRH = GPIO_Pin_1; // Low

#define OLED_NORESET()    GPIOC->BSRRL = GPIO_Pin_2; // High
#define OLED_RESET()      GPIOC->BSRRH = GPIO_Pin_2; // Low

#define OLED_VCC_ON()    GPIOB->BSRRL = GPIO_Pin_11; // High
#define OLED_VCC_OFF()   GPIOB->BSRRH = GPIO_Pin_11; // Low

#define OLED_WIDTH 96
#define OLED_HEIGHT 64

// SSD1331 Commands
#define SSD1331_CMD_DRAWLINE 0x21
#define SSD1331_CMD_DRAWRECT 0x22
#define SSD1331_CMD_CLEAR 0x25
#define SSD1331_CMD_FILL 0x26
#define SSD1331_CMD_SETCOLUMN 0x15
#define SSD1331_CMD_SETROW 0x75
#define SSD1331_CMD_CONTRASTA 0x81
#define SSD1331_CMD_CONTRASTB 0x82
#define SSD1331_CMD_CONTRASTC 0x83
#define SSD1331_CMD_MASTERCURRENT 0x87
#define SSD1331_CMD_SETREMAP 0xA0
#define SSD1331_CMD_STARTLINE 0xA1
#define SSD1331_CMD_DISPLAYOFFSET 0xA2
#define SSD1331_CMD_NORMALDISPLAY 0xA4
#define SSD1331_CMD_DISPLAYALLON 0xA5
#define SSD1331_CMD_DISPLAYALLOFF 0xA6
#define SSD1331_CMD_INVERTDISPLAY 0xA7
#define SSD1331_CMD_SETMULTIPLEX 0xA8
#define SSD1331_CMD_SETMASTER 0xAD
#define SSD1331_CMD_DISPLAYOFF 0xAE
#define SSD1331_CMD_DISPLAYON 0xAF
#define SSD1331_CMD_POWERMODE 0xB0
#define SSD1331_CMD_PRECHARGE 0xB1
#define SSD1331_CMD_CLOCKDIV 0xB3
#define SSD1331_CMD_PRECHARGEA 0x8A
#define SSD1331_CMD_PRECHARGEB 0x8B
#define SSD1331_CMD_PRECHARGEC 0x8C
#define SSD1331_CMD_PRECHARGELEVEL 0xBB
#define SSD1331_CMD_VCOMH 0xBE

#define OLED_BLACK 0x00
#define OLED_RED 0xE0
#define OLED_YELLOW 0xE8
#define OLED_DARK_YELLOW 0x64

#endif /* OLED_H_ */
