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

int view_schedule = 0;

// Displayed values
int voltage = 1234;
int current = 1005;

//use own printf
int sprintf(char *out, const char *format, ...);
char s[10];
/**
 * Initialize the module
 */
void view_init(void) {

	oled_init();
}

/**
 * The view task
 */
void view_task(void) {
	voltage = adc_getResult(ADC_CHAN_VIN);

	if (view_schedule == 0) {
		sprintf(s,"%02u.%01u",voltage / 100, (voltage % 100) / 10);
		oled_writeStringLarge (0,0,  s, OLED_RED);
		sprintf(s,"%01u", voltage % 10);
		oled_writeStringMedium (60,7,  s, OLED_RED);
		oled_writeStringSmall (76,14,  "Volt", OLED_RED);
	}

	if (view_schedule == 2) {
		sprintf(s,"%01u.%02u",current / 1000, (current % 1000) / 10);
		oled_writeStringLarge (0,24, s, OLED_YELLOW);
		sprintf(s,"%01u", current % 10);
		oled_writeStringMedium (60,31, s, OLED_YELLOW);
		oled_writeStringSmall (76,38, "Amps", OLED_YELLOW);
	}

	sprintf(s,"%02u.%01u",voltage_setp / 10, voltage_setp % 10);
	oled_writeStringSmall (76,0,  s, OLED_RED);
	sprintf(s,"%01u.%02u",current_setp / 100, current_setp % 100);
	oled_writeStringSmall (76,24, s, OLED_YELLOW);

	oled_drawScope ();


	view_schedule++;
	if (view_schedule >= 4)
		view_schedule = 0;
}
