/*
 *  Project:      Mini_PSU
 *  File:         rc.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for rc.c
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
#ifndef RC_H_
#define RC_H_
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"



// Function Prototypes
void rc_captured(uint16_t duty, uint16_t periode);
void rc_init(void);
void rc_set(int16_t value);
int16_t rc_get_channel(int channel);

// Constant values
#define RC_CHANNEL_Y 0
#define RC_CHANNEL_X 1
#define RC_CHANNEL_SPEED 2
#define RC_CHANNEL_TEST 3
#define RC_CHANNEL_SWITCH 4

#endif /* RC_H_ */
