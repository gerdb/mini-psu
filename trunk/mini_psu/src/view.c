/*
 *  Project:      Mini_PSU
 *  File:         view.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  View
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

#include "view.h"
#include "oled.h"
#include "adc.h"
#include "encoder.h"
#include "controller.h"
#include "data.h"



//use own printf
int sprintf(char *out, const char *format, ...);

/**
 * local variables
 */
char s[10];
int view_schedule = 0;
int setupControllerOld = -1;
/**
 * local functions
 */
static void view_DisplayNormal(void);
static void view_DisplayControllerSettings(void);

/**
 * Initialize the module
 */
void view_init(void) {

	oled_init();
}

/*
 * Display the standard screen with voltage and current settings
 */
static void view_DisplayNormal(void) {
	int vo = adc_ADCTo10mV(voltage_VOUT);
	int io = adc_ADCTo1mA(current_IOUT);
	int color_volt;
	int color_amps;

	// Color of the voltage and current values
	if (outputOn) {
		color_volt = OLED_RED;
		color_amps = OLED_YELLOW;
	} else {
		color_volt = OLED_YELLOW;
		color_amps = OLED_YELLOW;
	}

	if (view_schedule == 0) {
		sprintf(s,"%02u.%01u",vo / 100, (vo % 100) / 10);
		oled_writeStringLarge (0,0,  s, color_volt);
		sprintf(s,"%01u", vo % 10);
		oled_writeStringMedium (60,7,  s, color_volt);
		oled_writeStringSmall (76,14,  "Volt", color_volt);
	}

	if (view_schedule == 2) {
		sprintf(s,"%01u.%02u",io / 1000, (io % 1000) / 10);
		oled_writeStringLarge (0,24, s, color_amps);
		sprintf(s,"%01u", io % 10);
		oled_writeStringMedium (60,31, s, color_amps);
		oled_writeStringSmall (76,38, "Amps", color_amps);
	}

	sprintf(s,"%02u.%01u",voltage_setp / 10, voltage_setp % 10);
	oled_writeStringSmall (76,0,  s, color_volt);
	sprintf(s,"%01u.%02u",current_setp / 100, current_setp % 100);
	oled_writeStringSmall (76,24, s, color_amps);

	// Draw the scope
	oled_drawScope (15 * io /(current_setp*10) );
}

/*
 * Display the controller settings
 */
static void view_DisplayControllerSettings(void) {


	sprintf(s,"%3u", pid_KP);
	oled_writeStringMedium (0, 0,  s, cursor == CURSOR_KP ? OLED_RED : OLED_YELLOW);

	sprintf(s,"%3u", pid_KI);
	oled_writeStringMedium (0, 16,  s, cursor == CURSOR_KI ? OLED_RED : OLED_YELLOW);

	sprintf(s,"%3u", pid_KD);
	oled_writeStringMedium (0, 32,  s, cursor == CURSOR_KD ? OLED_RED : OLED_YELLOW);

	sprintf(s,"%3u", voltage_VSM);
	oled_writeStringMedium (0, 48,  s, OLED_YELLOW);


	// Debug output to help tuning the PID controller
	sprintf(s,"%4u", voltage_setpSM);
	oled_writeStringSmall (64, 0,  s, OLED_YELLOW);

	sprintf(s,"%4u", contr);
	oled_writeStringSmall (64, 8,  s, OLED_YELLOW);

	sprintf(s,"%4d", err);
	oled_writeStringSmall (64, 16,  s, OLED_YELLOW);

	sprintf(s,"%5u", scaleSetpointFilt);
	oled_writeStringSmall (64, 24,  s, OLED_YELLOW);

	sprintf(s,"%5d", integrator);
	oled_writeStringSmall (64, 32,  s, OLED_YELLOW);

	sprintf(s,"%5d", adc_ADCTo10mV(voltage_VOUT));
	oled_writeStringSmall (64, 40,  s, OLED_RED);

	sprintf(s,"%5d", adc_ADCTo10mV(voltage_VIN));
	oled_writeStringSmall (64, 48,  s, OLED_RED);

	sprintf(s,"%5d", current_IOUT);
	oled_writeStringSmall (64, 56,  s, OLED_RED);

}


/**
 * The view task
 */
void view_task(void) {

	// Clear display if screen was switched
	if (setupController != setupControllerOld) {
		oled_clear();
	}
	setupControllerOld = setupController;

	// Setup or main screen
	if (setupController) {
		view_DisplayControllerSettings();
	} else {
		view_DisplayNormal();
	}

	view_schedule++;
	if (view_schedule >= 4)
		view_schedule = 0;
}
