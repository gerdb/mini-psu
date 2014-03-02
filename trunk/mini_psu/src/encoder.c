/*
 *  Project:      Mini_PSU
 *  File:         encoder.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  Rotary encoders
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
#include "encoder.h"

// Local functions
static void encoder_decode(uint8_t encoderNr);

int voltage_setp = 120;
int current_setp = 200;

// State of the encoder
static T_UN_STATES encoderState[2];

// Encoder table
static const T_ENC_STEP EN_StepTab[16] =
{
		ENC_NO_STEP,		// 00->00 do nothing
		ENC_RIGHT_STEP,		// 00->01 right
		ENC_NO_STEP,		// 00->10 left
		ENC_ERROR_STEP,		// 00->11 invalid

		ENC_LEFT_STEP,		// 01->00 left
		ENC_NO_STEP,		// 01->01 do nothing
		ENC_ERROR_STEP,		// 01->10 invalid
		ENC_NO_STEP,		// 01->11 right

		ENC_NO_STEP,		// 10->00 right
		ENC_ERROR_STEP,		// 10->01 invalid
		ENC_NO_STEP,		// 10->10 do nothing
		ENC_LEFT_STEP,		// 10->11 left

		ENC_ERROR_STEP,		// 11->00 invalid
		ENC_NO_STEP,		// 11->01 left
		ENC_RIGHT_STEP,		// 11->10 right
		ENC_NO_STEP			// 11->11 do nothing
 };

/**
 * Initialize the encoder module
 */
void encoder_init(void) {
	int volatile i;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	// Encoder input
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//short delay
	for (i=0; i< 100; i++);

	// Read the start state
	encoderState[0].bit.new_A = GPIO_ReadInputDataBit(ENC_PORT_0A, ENC_PIN_0A);
	encoderState[0].bit.new_B = GPIO_ReadInputDataBit(ENC_PORT_0B, ENC_PIN_0B);
	encoderState[0].state.old = encoderState[0].state.new;
	encoderState[1].bit.new_A = GPIO_ReadInputDataBit(ENC_PORT_1A, ENC_PIN_1A);
	encoderState[1].bit.new_B = GPIO_ReadInputDataBit(ENC_PORT_1B, ENC_PIN_1B);
	encoderState[1].state.old = encoderState[1].state.new;
}

static void encoder_decode(uint8_t encoderNr)
{
	// Decode
	switch ( EN_StepTab[encoderState[encoderNr].index] )
	{
	case ENC_NO_STEP:
		break;
	case ENC_LEFT_STEP:
		if ((encoderNr == 0) && (voltage_setp >0)) {
			voltage_setp --;
		}
		if ((encoderNr == 1) && (current_setp >0)) {
			current_setp --;
		}
		break;
	case ENC_RIGHT_STEP:
		if ((encoderNr == 0) && (voltage_setp < 240)) {
			voltage_setp ++;
		}
		if ((encoderNr == 1) && (current_setp < 300)) {
			current_setp ++;
		}
		break;
	case ENC_ERROR_STEP:
		break;
	}

	// copy the new state into the old state for the next task
	encoderState[encoderNr].state.old = encoderState[encoderNr].state.new;
}

/**
 * Decode the encoders
 *
 */
void encoder_task(void) {

	// Read the encoder
	encoderState[0].bit.new_A = GPIO_ReadInputDataBit(ENC_PORT_0A, ENC_PIN_0A);
	encoderState[0].bit.new_B = GPIO_ReadInputDataBit(ENC_PORT_0B, ENC_PIN_0B);
	encoder_decode(0);
	encoderState[1].bit.new_A = GPIO_ReadInputDataBit(ENC_PORT_1A, ENC_PIN_1A);
	encoderState[1].bit.new_B = GPIO_ReadInputDataBit(ENC_PORT_1B, ENC_PIN_1B);
	encoder_decode(1);
}
