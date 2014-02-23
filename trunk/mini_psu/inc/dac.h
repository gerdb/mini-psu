/*
 *  Project:      Mini_PSU
 *  File:         dac.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for dac.c
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
#ifndef DAC_H_
#define DAC_H_

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dac.h"

// Function Prototypes
void dac_init(void);
void dac_setVoltageValue(uint32_t value);
void dac_setCurrentValue(uint32_t value);

#endif /* DAC_H_ */
