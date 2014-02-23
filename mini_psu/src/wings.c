/*
 *  Project:      Mini_PSU
 *  File:         controller.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  Controlls the wings based on the RC signals and
 *                the phase of the rotation module
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
#include <math.h>
#include "wings.h"
#include "rc.h"
#include "controller.h"
#include "rotation.h"

// Sine and cosine tables
int SINTAB[256];
int COSTAB[256];
/**
 * Initialize the wings module
 */
void wings_init(void) {
	int i;


	// fill the cosine and sine table
	for (i=0;i<256;i++) {
		COSTAB[i] = (int)(256.0*cos(M_TWOPI*(double)i/256.0));
		SINTAB[i] = (int)(256.0*sin(M_TWOPI*(double)i/256.0));
	}
}

/**
 * Controls the wings, depending on the system state,
 * the phase and the RC signal
 *
 * @param
 * 			sysstate System state
 */
void wings_task(e_sysstate sysstate){


	// phase of the rotation
	uint16_t rot_phase;
	// RC signals
	int rc_x,rc_y;
	int w0,w1,w2,w3;

	switch (sysstate) {

	// The wings are in the zero position
	case OFF:
	case IDLE:
		controller_setSetpoint(0,0);
		controller_setSetpoint(1,0);
		controller_setSetpoint(2,0);
		controller_setSetpoint(3,0);
		break;

	// Try to regulate the wings so, that the Mini_PSU stops
	// rotating
	case GYRO:

		// .. TODO

		controller_setSetpoint(0,0);
		controller_setSetpoint(1,0);
		controller_setSetpoint(2,0);
		controller_setSetpoint(3,0);
		break;

	// In SUN state, use the synthesized phase and the RC signals to
	// control the wings
	case SUN:
	case SUNSIMULATION:
		// Get the phase and the RC signals
		rot_phase = rotation_getPhase();
		rc_x = rc_get_channel(RC_CHANNEL_X);
		rc_y = rc_get_channel(RC_CHANNEL_Y);

		// Calculate the position of the wings
		w0 = rc_x * SINTAB[rot_phase] / 32;
		w1 = rc_y * COSTAB[rot_phase] / 32;
		w2 = -w0;
		w3 = -w1;

		// Set the wings
		controller_setSetpoint(0, w0);
		controller_setSetpoint(1, w1);
		controller_setSetpoint(2, w2);
		controller_setSetpoint(3, w3);

		break;
	}

}
