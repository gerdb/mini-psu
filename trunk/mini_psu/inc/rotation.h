/*
 *  Project:      Mini_PSU
 *  File:         rotation.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for rotation.c
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
#ifndef ROTATION_H_
#define ROTATION_H_

#include "stm32f4xx.h"

// Function Prototypes
void rotation_init(void);
void rotation_task(void);
int rotation_getAngle(void);
uint16_t rotation_getPhase(void);
void rotation_useSimulation(int sim);
#endif /* ROTATION_H_ */
