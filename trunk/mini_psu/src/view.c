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
#include "data.h"



//use own printf
int sprintf(char *out, const char *format, ...);

/**
 * local variables
 */
char s[10];
int view_schedule = 0;

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
	if (view_schedule == 0) {
		sprintf(s,"%02u.%01u",voltage_VOUT / 100, (voltage_VOUT % 100) / 10);
		oled_writeStringLarge (0,0,  s, OLED_RED);
		sprintf(s,"%01u", voltage_VOUT % 10);
		oled_writeStringMedium (60,7,  s, OLED_RED);
		oled_writeStringSmall (76,14,  "Volt", OLED_RED);
	}

	if (view_schedule == 2) {
		sprintf(s,"%01u.%02u",current_IOUT / 1000, (current_IOUT % 1000) / 10);
		oled_writeStringLarge (0,24, s, OLED_YELLOW);
		sprintf(s,"%01u", current_IOUT % 10);
		oled_writeStringMedium (60,31, s, OLED_YELLOW);
		oled_writeStringSmall (76,38, "Amps", OLED_YELLOW);
	}

	sprintf(s,"%02u.%01u",voltage_setp / 10, voltage_setp % 10);
	oled_writeStringSmall (76,0,  s, OLED_RED);
	sprintf(s,"%01u.%02u",current_setp / 100, current_setp % 100);
	oled_writeStringSmall (76,24, s, OLED_YELLOW);

	oled_drawScope ();
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

	sprintf(s,"%3u", voltage_setpSM);
	oled_writeStringSmall (78, 0,  s, OLED_YELLOW);
}


/**
 * The view task
 */
void view_task(void) {

	if (setupController) {
		view_DisplayControllerSettings();
	} else {
		view_DisplayNormal();
	}

	view_schedule++;
	if (view_schedule >= 4)
		view_schedule = 0;
}
