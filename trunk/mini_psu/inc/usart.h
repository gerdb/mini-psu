/*
 *  Project:      Mini_PSU
 *  File:         usart.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for usart.c
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
#ifndef USART_H_
#define USART_H_

#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"

// Function Prototypes
void usart_init(void);
void usart_putchar(int ch);
void usart_buffertask(void);
int my_printf(const char *format, ...);

#endif /* USART_H_ */
