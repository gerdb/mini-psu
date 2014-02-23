/*
 *  Project:      Mini_PSU
 *  File:         rc.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  Decodes the signal from the RC receiver
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
#include "rc.h"
#include "misc.h"

// Result of the 6 RC channels
uint16_t rc_in_sigs[6] = {300,300,300,300,300,300};
uint16_t rc_in_ix=0;

/**
 * Initializer of the RC module
 */
void rc_init(void) {

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

    // GPIOA clock enable
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // Initialize PA8, Alternative Function, 100Mhz, Output, Push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM10);

    // TIM10 clock enable
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10 , ENABLE);

    // Timer Base configuration
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 168*5-1; // 5us;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 4000; //20ms
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);

    // Channel 1 output configuration
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 300;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OC1Init(TIM10, &TIM_OCInitStructure);

    // Very much needed.  Enable Preload register on CCR1.
    TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);

    // TIM1 counter enable
    TIM_Cmd(TIM10, ENABLE);

    // TIM1 Main Output Enable
    TIM_CtrlPWMOutputs(TIM10, ENABLE);


	// TIM1 clock enable
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	// GPIOB clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	// TIM1 chennel2 configuration : PB.07
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	// Connect TIM pin to AF2
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);

	// Enable the TIM1 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Timer Base configuration
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 168*5-1; // 5us
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 10000; // 50ms
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;

	TIM_PWMIConfig(TIM1, &TIM_ICInitStructure);

	// Select the TIM1 Input Trigger: TI2FP2
	TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);

	// Select the slave Mode: Reset Mode
	TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(TIM1,TIM_MasterSlaveMode_Enable);

	// TIM enable counter
	TIM_Cmd(TIM1, ENABLE);

	// Enable the CC2 Interrupt Request
	TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);

}

/**
 * Set the output channel (to the motor controller)
 * with a specified value
 *
 * @param
 * 		value the new motor speed from -100 to +200
 *
 */
void rc_set(int16_t value) {
	TIM_SetCompare1(TIM10, 300+value);
}

/**
 * IRQ callback function that decodes the duty and the
 * periode and sort it into the channel number
 */
void rc_captured(uint16_t duty, uint16_t periode) {

	// If the periode is >600 (3ms), it is a new sequence
	if (periode > 600 ) {
		rc_in_ix = 0;
	}
	// Put the value into the next field of the array
	else if (rc_in_ix < 6) {
		rc_in_sigs[rc_in_ix] = periode;
		rc_in_ix++;
	}

}

/**
 * Getter for the RC value
 *
 * @param
 * 			channel selected channel
 * @return
 * 			Value of the RC channel. 1ms = 200, 2ms = 400
 */
int16_t rc_get_channel(int channel) {
	return rc_in_sigs[channel]-300;
}
