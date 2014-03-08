/*
 *  Project:      Mini_PSU
 *  File:         encoder.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for encoder.c
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
#ifndef ENCODER_H_
#define ENCODER_H_

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

// Function Prototypes
void encoder_init(void);
void encoder_task(void);

// Encoder steps
typedef enum {
	ENC_NO_STEP, ENC_RIGHT_STEP, ENC_LEFT_STEP, ENC_ERROR_STEP
} T_ENC_STEP;

//Encoder structure
typedef union {
	uint8_t index;
	struct {
		uint8_t new :2;
		uint8_t old :2;
	} state;
	struct {
		uint8_t new_A :1;
		uint8_t new_B :1;
		uint8_t old_A :1;
		uint8_t old_B :1;
	} bit;
} T_UN_STATES;

#define ENC_PORT_0A GPIOC
#define ENC_PORT_0B GPIOC
#define ENC_PORT_0S GPIOC
#define ENC_PORT_1A GPIOB
#define ENC_PORT_1B GPIOB
#define ENC_PORT_1S GPIOB

#define ENC_PIN_0A GPIO_Pin_8
#define ENC_PIN_0B GPIO_Pin_7
#define ENC_PIN_0S GPIO_Pin_6
#define ENC_PIN_1A GPIO_Pin_15
#define ENC_PIN_1B GPIO_Pin_14
#define ENC_PIN_1S GPIO_Pin_13
#endif /* ENCODER_H_ */
