/*
 *  Project:      Mini_PSU
 *  File:         spi.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for spi.c
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
#ifndef SPI_H_
#define SPI_H_
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"
#include "misc.h"

// Function Prototypes
void spi_init(void);
void spi_send(uint8_t byte );

// Macros
#define SPI_CS_DISABLE() GPIOB->BSRRL = GPIO_Pin_12;
#define SPI_CS_ENABLE()  GPIOB->BSRRH = GPIO_Pin_12;


#endif /* SPI_H_ */
