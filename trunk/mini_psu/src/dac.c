/*
 *  Project:      Mini_PSU
 *  File:         dac.c
 *  Author:       Gerd Bartelt - www.sebulli.com - www.sebulli.com
 *
 *  Description:  Used the DAC to output analog debug values
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
#include "dac.h"
#include "project.h"

/**
 * Initialize the DAC
 */
void dac_init(void) {

	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitStructure;

	//GPIOA clock enable (to be used with DAC)
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// DAC Periph clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	// DAC port (DAC_OUT1 = PA.4 and DAC_OUT2 = PA.5) configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// DAC channel 1 configuration
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	// Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
	// automatically connected to the DAC converter.
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);

	// Set DAC channel register to 0
	DAC_SetChannel1Data(DAC_Align_12b_R, 0);
	DAC_SetChannel2Data(DAC_Align_12b_R, 0);
}
/**
 * Sets the DAC for voltage set point
 * Voltage divider is 10k:1k, or /11
 *
 * @param
 * 			value voltage set point from 0.. 24000 in mV steps
 */
void dac_setVoltageValue(uint32_t value) {
	// Voltage divider in amplifier feedback is 10k:1k
	// Additional voltage divider R29/R31 of 100R/56k = 0,998217469
	// So the gain is set to x11
	// Reference voltage is 3.3V
	// Vout = DAC / 4096 * 3.3V * 11 * 0.998217469
	// DAC = Vout / 11 / 3.3V / 0.998217469 * 4096
	// DAC = Vout(V) * 113.038961039
	// DAC = Vout(mV) * 0.113038961039
	// DAC = ca. Vout(mV) * 463 / (4096)
	DAC_SetChannel1Data(DAC_Align_12b_R, (value * 463) / 4096);
}

/**
 * Sets the DAC for current set point
 *
 * @param
 * 			value current set point from 0.. 3000 in mA steps
 */
void dac_setCurrentValue(uint32_t value) {
	// Shunt resistor: 10mR
	// Current ampifier: x51,6961
	// DAC = I * Rshunt * 51,6961 / Vref * 12Bit
	// DAC = I * 10mR * 51,6961 / 3,3V * 4096
	// DAC = I(mA) * 0,641658259
	// DAC = ca. 657 / 1024
	DAC_SetChannel2Data(DAC_Align_12b_R, (value * 657) / 1024);
}
