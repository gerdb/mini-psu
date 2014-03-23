/*
 *  Project:      Mini_PSU
 *  File:         controller.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for controller.c
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
#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"

//global values (debugging)
extern int contr;
extern int err;
extern int scaleSetpointFilt;
extern int integrator;
// Function Prototypes
void controller_init(void);
inline void controller_task(void);
void controller_SlowTask(void);

//defines
#define DEBUG_PIN GPIO_Pin_9
#define DEBUG_PORT GPIOC

#endif /* CONTROLLER_H_ */
