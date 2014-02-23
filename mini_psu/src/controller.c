/*
 *  Project:      Mini_PSU
 *  File:         controller.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  PD controller for the high speed actuators
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


#include "controller.h"
#include "adc.h"
#include "pwm.h"


// How the analog channels are mapped
int AD_CHANNEL_MAP[4][2];
// Setpoints
int32_t setpoint[4]={0,0,0,0};

// The controller parameters
int32_t KP = 120; //120
int32_t KD = 300; //-300
#define CONTR_LIMIT 1000

/**
 * Initialize the controller and configure the analog channels
 */
void controller_init(void) {

	// Create a table with the index of all analog channels
	AD_CHANNEL_MAP[0][0]=3;
	AD_CHANNEL_MAP[0][1]=4;

	AD_CHANNEL_MAP[1][0]=2;
	AD_CHANNEL_MAP[1][1]=5;

	AD_CHANNEL_MAP[2][0]=1;
	AD_CHANNEL_MAP[2][1]=6;

	AD_CHANNEL_MAP[3][0]=0;
	AD_CHANNEL_MAP[3][1]=7;

}
/**
 * Setter for the controllers setpoint
 *
 * @param
 * 			channel Channel no from 0 to 3
 * 			setp  Setpoint from -1000 to +1000
 *
 */
void controller_setSetpoint(int channel, int setp) {
	setpoint[channel] = setp;
}

/**
 * Controller task
 * Call this every 100µs
 *
 * @param
 * 			channel Channel no from 0 to 3
 *
 */
void controller_task(int channel){

	int32_t meas;
	static int32_t meas_old[4]={0,0,0,0};
	int32_t rdiff;
	int32_t reg = 0;

	// Get the sensor value.
	meas = adc_getResult(AD_CHANNEL_MAP[channel][0]) - adc_getResult(AD_CHANNEL_MAP[channel][1]);

	// Calculate the difference between setpoint and sensor value
	rdiff = setpoint[channel] - meas;

	// The PD regulator with a scaling factor
	reg = rdiff *KP + (meas_old[channel]-meas) * KD;
	reg /= 32;

	// Store the old value for the D-value
	meas_old[channel] = meas;

	// Limit the value
	if (reg > +CONTR_LIMIT) reg = +CONTR_LIMIT;
	if (reg < -CONTR_LIMIT) reg = -CONTR_LIMIT;

	// Set the PWM output
	pwm_set(reg);
}

