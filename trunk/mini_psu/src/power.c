/*
 *  Project:      Mini_PSU
 *  File:         power.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  Measures the battery voltage and sets under voltage errors
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
#include "power.h"
#include "adc.h"
#include "light.h"
#include "typedef.h"

// filterer value of the battery voltage
uint32_t ubat_filt = 0;
uint32_t ubat_filtL = 0;

// Power up counter
int powerUpCnt = 0;

// Filter time of UBAT
#define UBAT_FILT 8192
// Limits of the under voltage detection
#define LOW_BAT_L 10500
#define LOW_BAT_U 11000

// Flag for low battery
int lowbat = 0;


/**
 * Initialize the power module
 */
void power_init(void) {

	//Wait the first 10ms for stabilization
	powerUpCnt = 100;
	lowbat = 0;
}

/**
 * Getter for the battery voltage
 *
 * @return
 * 		The battery voltage in mV
 */
uint32_t power_getUbat() {
	// 3.3V
	// Voltage divider 10:2k35 = 1:5,255319149
	// 3300 / 4096 *5,255.. = 271 / 64
	return adc_getResult(ADC_CHAN_BATT) * 271/64;
}

/**
 * Filter the battery voltage
 */
void power_task(void){

	// In the power up phase, do not filter,
	// But preload the filter
	if (powerUpCnt > 0) {
		ubat_filt = power_getUbat();
		ubat_filtL = ubat_filt * UBAT_FILT;
		powerUpCnt--;
	}
	else {

		// Low pass filter
		ubat_filtL += power_getUbat() - ubat_filt;
		ubat_filt = ubat_filtL / UBAT_FILT;

		// Set and reset low batt warning
		if (ubat_filt < LOW_BAT_L) lowbat = 1;
		if (ubat_filt > LOW_BAT_U) lowbat = 0;
	}

	// Show the low bat error
	if (lowbat) {
		light_set_error(LOWBAT);
	}

}

/**
 * Getter for the filtered battery voltage
 *
 * @return
 * 		The filtered battery voltage in mV
 */
uint32_t power_getUbatFilt() {
	return ubat_filt;
}

