/*
 *  Project:      Mini_PSU
 *  File:         controller.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  Calculates the angle based on the polarized light
 *                or the gyro sensor
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
#include "rotation.h"
#include "spi.h"
#include "adc.h"
#include "dac.h"


// gyro integrator
int32_t integrator=0;
// Offset of the gyro sensor
int32_t gyro_offset=-8;
// Low pass filter of the sun signal
int pol_lp_filt = 0;
int32_t pol_lp_filtL = 0;
// High pass filter of the sun signal
int pol_hp_filt = 0;
// buffer of the FIR filter
int fir_buf[256];
// FIR index counter
int fir_cnt = 0;
// Sum of the FIR filter
int32_t fir_sum = 0;
// Sign of the sun light detection
int firSign = 0;
int firSign_old = 0;
// Counter to suppredd the zero detection
int fir_supressSignCnt = 0;
// The timestamp of the edge detection
uint16_t posEdge = 0;
uint16_t negEdge = 0;
// Periode time
uint16_t rotperiode = 0;
// Timestamp of the median point (between pos and neg edge)
uint16_t median = 0;
// Synthesized phase
uint16_t phasecnt = 0;
uint16_t relphasecnt = 0;
uint32_t phase = 0;
uint32_t simuphase = 0;
int use_sim = 0;

// filter value, 8192 = 819ms
#define POLFILT 8192
// suppress a new zero detection for 10ms = 100
#define SUPPRESSSIGN 100
#define MAXSIGNCNT 30000

/**
 * Initialize the rotation module
 */
void rotation_init(void) {

	int i;

	// reset the FIR filter and fill the cosine and sine table
	for (i=0;i<256;i++) {
		fir_buf[i] = 0;
	}

	fir_sum = 0;
	fir_cnt = 0;
}


/**
 * Integrates the gyro, detects the sun light signals
 * and generates a new signal, that is in phase with
 * the sun signal
 */
void rotation_task(void) {

	//The actual sun signal
	int pol_act;

	// Integrate the gyro signal
	integrator+= spi_getGyro() + gyro_offset;

	// get value of polarization sensor
	pol_act = adc_getResult(ADC_CHAN_POL);

	// low pass filter
	pol_lp_filtL += pol_act-pol_lp_filt;
	pol_lp_filt = pol_lp_filtL / POLFILT;

	// high pass filter
	pol_hp_filt = pol_act - pol_lp_filt;

	// FIR filter
	fir_sum+= pol_hp_filt;
	fir_buf[fir_cnt] = pol_hp_filt;
	fir_cnt++;
	fir_cnt &= 0x00ff;
	fir_sum-= fir_buf[fir_cnt];

	// Create ta new phase based on this counter
	phasecnt++;

	// Detect a zero crossing
	firSign = (fir_sum > 0);

	// Suppress a detection of a new zero crossing for 10ms
	if (fir_supressSignCnt < SUPPRESSSIGN) {
		fir_supressSignCnt++;
	}
	else {

		// positive edge detected
		if ((firSign == !0) && (firSign_old == 0) ) {

			// Store counter at the positive edge
			posEdge = phasecnt;
			fir_supressSignCnt = 0;
		}
		// negative edge detected
		if ((firSign_old == !0) && (firSign == 0) ) {

			// Store counter at the negative edge
			// and calculate the periode time
			rotperiode =  phasecnt - negEdge;
			negEdge = phasecnt;

			// Calculate the point in the middle between
			// the positive and negative edge
			if (negEdge >= posEdge) {
				median = ((uint32_t)negEdge + (uint32_t)posEdge) / 2;
			}
			else {
				median = (((uint32_t)negEdge + 65536L + (uint32_t)posEdge) / 2) & 0x0000ffff;
			}
			fir_supressSignCnt = 0;
		}

	}

	// The pase is relative to the median point and
	// corrected by half of the FIR delay
	relphasecnt = phasecnt - median + 128;

	// The phase is limtted to values from 0..255 = 0..359deg
	phase = (relphasecnt * 256 / rotperiode) & 0x00FF;

	// simulate a phase
	simuphase += use_sim;

	firSign_old = firSign;

	// Debug
	// Scale 0..256 to 0..2048 (11Bit)
	// dac_setValue((256+COSTAB[phase])*4);


}

/**
 * Getter for the rotation phase based on the sun sensor
 *
 * @return
 * 			The phase from 0..255
 */
uint16_t rotation_getPhase(void) {
	if (use_sim)
		return (simuphase / 128) & 0x00FF;
	else
		return phase;
}

/**
 * Getter for the rotation phase based on the gyro sensor
 *
 * @return
 * 			The integrator value
 */
int rotation_getAngle(void) {
	return integrator;
}

/**
 * Setter for the simulation flag
 *
 * @param
 * 			sim >0, if simulated value should be used
 */
void rotation_useSimulation(int sim) {
	use_sim = sim;
}
