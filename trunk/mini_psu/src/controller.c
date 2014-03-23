/*
 *  Project:      Mini_PSU
 *  File:         controller.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  PID controller
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
#include "data.h"

volatile int i;
int contr;
int err;
int integrator = 0;
int limit_up = 0;
int limit_down = 0;
int scaleSetpoint = 0;
int scaleSetpointFilt2 = 0;
int scaleSetpointFilt2L = 0;
int scaleSetpointFilt = 0;
int scaleSetpointFiltL = 0;
int setpointFilt = 0;
int setpointFiltL = 0;
int voltage_VSM_old = 0;
/**
 * Initialize the controller and configure the analog channels
 */
void controller_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	// Debug pin
	GPIO_InitStructure.GPIO_Pin = DEBUG_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(DEBUG_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(DEBUG_PORT, DEBUG_PIN);



	// Enable the TIM2 gloabal Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// TIM2 clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	// Time base configuration
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1; // 84kHz
	TIM_TimeBaseStructure.TIM_Prescaler = 1 - 1; // 84 MHz Clock
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	// TIM IT enable
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//
	TIM_Cmd(TIM2, ENABLE);

}


/**
 * Controller task
 * Call this every 10µs
 *
 *
 */
inline void controller_task(void){

	GPIO_SetBits(DEBUG_PORT, DEBUG_PIN);

	// Read the ADC
	voltage_VOUT  = ADCConvertedValue[ADC_CHAN_VOUT];
	voltage_VSM   = ADCConvertedValue[ADC_CHAN_VSM];
	// Restart ADC
	ADC_SoftwareStartConv(ADC1);

	setpointFiltL += voltage_setpSM - setpointFilt;
	setpointFilt = setpointFiltL >> 2;

	err = setpointFilt - voltage_VSM;

	if (voltage_setpSM) {
		if ((err > 0) && (!limit_up)) {
			integrator += pid_KI * err;
		}
		if ((err < 0) && (!limit_down)) {
			integrator += pid_KI * err;
		}
		// Limit integrator
		if (integrator > 16384)
			integrator = 16384;
		if (integrator < -16384)
			integrator = -16384;
	} else {
		integrator = 0;
	}

	// calculate P and D part
	err = (setpointFilt * scaleSetpointFilt) / 4096 - voltage_VSM;

	contr =   pid_KP * err
			- pid_KD * (voltage_VSM-voltage_VSM_old)
			+ integrator / 4;

	voltage_VSM_old = voltage_VSM;

	contr /= 128;


	// Limit output value
	limit_down = 0;
	limit_up = 0;

	if (contr >= 499) {
		limit_up = 1;
		contr = 499;
	}

	if (contr < 0) {
		limit_down = 1;
		contr = 0;
	}


	// Controller output
	if (voltage_setpSM > 0) {
		//pwm_set(120);
		pwm_set(contr);
	}
	else {
		pwm_set(0);
	}

	GPIO_ResetBits(DEBUG_PORT, DEBUG_PIN);	// duration: 3µs / period: 12µs
}

/**
 * Controller slow task
 * Call this every 1ms
 *
 *
 */
void controller_SlowTask(void){
	if (outputOn)
		voltage_setpSM = voltage_VOUT + adc_VoltToADC(1000);
	else
		voltage_setpSM = 0;

	if (voltage_VIN > 1354) { // > 12V
		// Calculate the scaling factor of the setpoint
		// 4096: *1 (scaled by 4096)
		// 500: PWM resolution
		// 64: scaling factor of the controller
		// +4096: +1
		scaleSetpoint = 4096 * 500 * 128 / pid_KP / voltage_VIN + 4096;

		// Filter it with a low pass filter
		scaleSetpointFilt2L += scaleSetpoint - scaleSetpointFilt2;
		scaleSetpointFilt2 = scaleSetpointFilt2L / 256;

		scaleSetpointFiltL += scaleSetpointFilt2 - scaleSetpointFilt;
		scaleSetpointFilt = scaleSetpointFiltL / 256;
	}
}
