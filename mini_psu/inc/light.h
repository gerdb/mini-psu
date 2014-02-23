/*
 *  Project:      Mini_PSU
 *  File:         light.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for light.c
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
#ifndef LIGHT_H_
#define LIGHT_H_

#include "stm32f4xx.h"
#include "typedef.h"

// Function Prototypes
void light_init(void);
void light_power_led(int p_on);
void light_task(e_sysstate sysstate);
void light_set_error(e_errstate e);

// Constant values
#define ON 1
#define OFF 0

#define LED_G_1 GPIO_Pin_0
#define LED_R_1 GPIO_Pin_1
#define LED_G_2 GPIO_Pin_2
#define LED_R_2 GPIO_Pin_3

#define LED_ON(x) GPIOE->BSRRL = x
#define LED_OFF(x) GPIOE->BSRRH = x

#endif /* LIGHT_H_ */
