/*
 *  Project:      Mini_PSU
 *  File:         power.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for power.c
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
#ifndef POWER_H_
#define POWER_H_

#include "stm32f4xx.h"

// Function Prototypes
void power_init(void);
void power_task(void);
uint32_t power_getUbat();
uint32_t power_getUbatFilt();
int power_getLowBat();

#endif /* POWER_H_ */
