/*
 *  Project:      Mini_PSU
 *  File:         light.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  Controls the LEDs and used them to display
 *                the rotation angle or error states
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
#include "light.h"
#include "rotation.h"

// variables of this module
GPIO_InitTypeDef  GPIO_InitStructure;
e_sysstate sysstate_old = OFF;

// Counts the error flashs
int error_cnt=0;
// Timer for the error flashs
int error_tim=0;
// The error state
e_errstate error = OK;
e_errstate new_error = OK;
// Timer for the system-change-flash
int light_m_cnt = 0;

/**
 * Initialize the light module
 */
void light_init(void){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOB, ENABLE);

	/* Configure PB9 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//Switch on yellow power LED
	GPIOB->BSRRL = GPIO_Pin_9;

	/* Configure PE0..3 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = LED_G_1 | LED_R_1 |LED_G_2 |LED_R_2 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	LED_OFF(LED_G_1 | LED_R_1 |LED_G_2 |LED_R_2) ;

	/* Configure PA9 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIOA->BSRRL = GPIO_Pin_8;
	GPIOA->BSRRL = GPIO_Pin_9;
	GPIOA->BSRRL = GPIO_Pin_10;
	GPIOA->BSRRL = GPIO_Pin_11;
	GPIOA->BSRRL = GPIO_Pin_12;
}

/**
 * Set an error
 * The light will flash with the number of the error state
 *
 * @param
 *			e Errorstate
 */
void light_set_error(e_errstate e) {

	// The the error to the error with the
	// highest priority
	if (e != OFF) {
		if ((new_error == OFF) || (e < new_error)) {
			new_error = e;
		}
		if ((error == OFF) || (e < error)) {
			error = e;
		}
	}
}

/**
 * Setter for the yellow power LED
 *
 * @param
 * 			p_on state of the power led (ON or OFF)
 */
void light_power_led(int p_on) {
	if (p_on == ON) {
		// swtich on LED
		GPIOB->BSRRL = GPIO_Pin_9;
	}
	else {
		// swtich off LED
		GPIOB->BSRRH = GPIO_Pin_9;
	}
}

/**
 * Set the LEDs, depending on the system state
 *
 * @param
 * 			sysstate System state
 */
void light_task(e_sysstate sysstate) {

	// phase of the rotation
	uint16_t rot_phase;

	// let the LEDs flash with the number of the error
	if (error != OK) {

		// Flash frequency is 350ms
		error_tim++;
		if (error_tim > 3500) { // 350ms
			error_tim=0;
			error_cnt++;

			// wait for 5 cyles for the next flash sequence
			if (error_cnt>error) {
				error_cnt = -5;
				// take the new error
				error = new_error;
				new_error = OK;
			}
		}

		// Flash the LEDs
		// beause normaly they are ON, let them flash "OFF"
		if (error_tim > 1000 && error_cnt> 0) {
			LED_OFF(LED_R_1 | LED_R_2 | LED_G_1 | LED_G_2); // switch off LEDs
			return;
		}

	}

	// Detect a change of the system state
	if (sysstate!=sysstate_old) {
		light_m_cnt = 2500; // 250ms
		sysstate_old = sysstate;
	}

	// Flash the LEDs one time, if the system state changes
	if (light_m_cnt>0) {
		light_m_cnt--;
		LED_OFF(LED_R_1 | LED_R_2 | LED_G_1 | LED_G_2); // switch off LEDs
		return;
	}

	// Switch on/off the LEDs, depending on the system state
	switch (sysstate) {

	// All LEDs are off
	case OFF:
		LED_OFF(LED_R_1 | LED_R_2 | LED_G_1 | LED_G_2); // switch off LEDs
		break;

	// In IDLE state, the green LEDs are on
	case IDLE:
		LED_ON(LED_G_1 | LED_G_2);
		LED_OFF(LED_R_1 | LED_R_2);
		break;

	// In GYRO state, the LEDs are flashing very fast, but in phase
	// with the rotation angle, from the rotation sensor
	case GYRO:
		if (rotation_getAngle() & 0x00100000) {
			LED_ON(LED_R_1 | LED_R_2);
			LED_OFF(LED_G_1 | LED_G_2);
		}
		else {
			LED_ON(LED_G_1 | LED_G_2);
			LED_OFF(LED_R_1 | LED_R_2);
		}
		break;

	// In SUN State, the LEDs are green, of they are in front of
	// the Mini_PSU. And red on the rear side.
	case SUN:
	case SUNSIMULATION:
		rot_phase = rotation_getPhase();
		if (rot_phase < 128) {
			LED_ON(LED_G_1 | LED_R_2);
			LED_OFF(LED_R_1 | LED_G_2);
		}
		else {
			LED_ON(LED_R_1 | LED_G_2);
			LED_OFF(LED_G_1 | LED_R_2);
		}

		break;
	}

}
