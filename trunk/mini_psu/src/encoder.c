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
#include "data.h"

// Local functions
static void encoder_decode(uint8_t encoderNr);
static void encoder_step(uint8_t encoderNr, T_ENC_STEP direction);
static void encoder_click(uint8_t encoderNr);

// State of the encoder
static T_UN_STATES encoderState[2];
static int encoderSwitch[2] = {0,0};
static int encoderSwitchCnt[2] = {0,0};
static int encoderSwitchDebounced[2] = {0,0};

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

/*
 * Decodes encoder's push button
 */
static void encoder_click(uint8_t encoderNr) {
	if (setupController) {
		if (encoderNr == 1) {
			cursor ++;
			if (cursor >= 3)
				cursor = 0;
		}
	}
}

static void encoder_step(uint8_t encoderNr, T_ENC_STEP direction) {
	if (setupController) {
		if (encoderNr == 0) {

		}
		if (encoderNr == 1) {
			if (cursor  == CURSOR_KP) {
				if ((direction == ENC_LEFT_STEP) && (pid_KP > 0)) {
					pid_KP --;
				}
				if ((direction == ENC_RIGHT_STEP) && (pid_KP < 100)) {
					pid_KP ++;
				}
			}

			if (cursor  == CURSOR_KI) {
				if ((direction == ENC_LEFT_STEP) && (pid_KI > 0)) {
					pid_KI --;
				}
				if ((direction == ENC_RIGHT_STEP) && (pid_KI < 100)) {
					pid_KI ++;
				}
			}

			if (cursor  == CURSOR_KD) {
				if ((direction == ENC_LEFT_STEP) && (pid_KD > 0)) {
					pid_KD --;
				}
				if ((direction == ENC_RIGHT_STEP) && (pid_KD < 100)) {
					pid_KD ++;
				}
			}
		}
	} else {

		if (encoderNr == 0) {
			if ((direction == ENC_LEFT_STEP) && (voltage_setp >0)) {
				voltage_setp --;
			}
			if ((direction == ENC_RIGHT_STEP) && (voltage_setp < 240)) {
				voltage_setp ++;
			}

		}
		if (encoderNr == 1) {
			if ((direction == ENC_LEFT_STEP) && (current_setp >0)) {
				current_setp --;
			}
			if ((direction == ENC_RIGHT_STEP) && (current_setp < 300)) {
				current_setp ++;
			}
		}
	}
}

static void encoder_decode(uint8_t encoderNr)
{

	// debounce switch

	if (encoderSwitch[encoderNr] == 0 ) {
		if (encoderSwitchCnt[encoderNr]<10)
			encoderSwitchCnt[encoderNr]++;
		else {
			if (encoderSwitchDebounced[encoderNr] != 1) {
				encoderSwitchDebounced[encoderNr] = 1;
				encoder_click(encoderNr);
			}
		}
	} else  {
		if (encoderSwitchCnt[encoderNr]>0)
			encoderSwitchCnt[encoderNr]--;
		else {
			if (encoderSwitchDebounced[encoderNr] != 0) {
				encoderSwitchDebounced[encoderNr] = 0;
			}
		}
	}


	// Decode rotary encoder
	switch ( EN_StepTab[encoderState[encoderNr].index] )
	{
	case ENC_NO_STEP:
		break;
	case ENC_LEFT_STEP:
		encoder_step(encoderNr, ENC_LEFT_STEP);
		break;
	case ENC_RIGHT_STEP:
		encoder_step(encoderNr, ENC_RIGHT_STEP);
		break;
	case ENC_ERROR_STEP:
		break;
	}

	// copy the new state into the old state for the next task
	encoderState[encoderNr].state.old = encoderState[encoderNr].state.new;

	// Generate a step to tune the controller
	if (setupController) {
		if (encoderSwitchDebounced[0])
			voltage_setpSM = 120;
		else
			voltage_setpSM = 0;
	}
}

/**
 * Decode the encoders
 *
 */
void encoder_task(void) {

	// Read the encoder
	encoderState[0].bit.new_A = GPIO_ReadInputDataBit(ENC_PORT_0A, ENC_PIN_0A);
	encoderState[0].bit.new_B = GPIO_ReadInputDataBit(ENC_PORT_0B, ENC_PIN_0B);
	encoderSwitch[0] = GPIO_ReadInputDataBit(ENC_PORT_0S, ENC_PIN_0S);
	encoder_decode(0);
	encoderState[1].bit.new_A = GPIO_ReadInputDataBit(ENC_PORT_1A, ENC_PIN_1A);
	encoderState[1].bit.new_B = GPIO_ReadInputDataBit(ENC_PORT_1B, ENC_PIN_1B);
	encoderSwitch[1] = GPIO_ReadInputDataBit(ENC_PORT_1S, ENC_PIN_1S);
	encoder_decode(1);


}
