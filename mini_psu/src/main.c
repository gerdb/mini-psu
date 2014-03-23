/*
 *  Project:      Mini_PSU
 *  File:         main.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  main programm
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

#include "stm32f4xx.h"

#include "project.h"
#include "main.h"
#include "adc.h"
#include "pwm.h"
#include "dac.h"
#include "view.h"
#include "encoder.h"
#include "controller.h"
#include "data.h"

/*
 * local variables
 */
int tick = 0;
int tick50ms = 0;
int tick50msCnt = 0;
int tick100ms = 0;
int tick100msCnt = 0;


/*
 * Main function
 */
int main(void) {

	int i;

	// initialize all modules
	adc_init();
	dac_init();
	pwm_init();
	encoder_init();
	controller_init();

	// set sys tick to 1ms
	SysTick_Config(SystemCoreClock / 1000);

	// Wait 1sec
	for (i=0; i< 10;i++) {
		while (tick100ms == 0);
		tick100ms = 0;
	}

	// Init view and display after 1 sec
	view_init();

	while (1) {
		// wait for task tick
		while (tick == 0);
		tick = 0;
		if (tick50ms) {
			tick50ms = 0;
			view_task();
		}
	}
}



void setTick(void) {
	tick = 1;
	tick50msCnt++;
	tick100msCnt++;
	if (tick50msCnt>=50) {
		tick50msCnt = 0;
		tick50ms = 1;
	}
	if (tick100msCnt>=100) {
		tick100msCnt = 0;
		tick100ms = 1;
	}

	// 1ms Task
	adc_task();
	dac_task();
	controller_SlowTask();
	encoder_task();
}
