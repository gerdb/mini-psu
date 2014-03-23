/*
 *  Project:      Mini_PSU
 *  File:         adc.c
 *  Author:       Gerd Bartelt - www.sebulli.com - www.sebulli.com
 *
 *  Description:  Configures and reads the analog digital converter
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
#include "adc.h"
#include "data.h"
#include "project.h"

// The result of the analog to digital conversion
__IO uint16_t ADCConvertedValue[4] = {0,0,0,0};

/**
 * Initializes the ADC
 */
void adc_init() {
	  ADC_InitTypeDef       ADC_InitStructure;
	  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	  DMA_InitTypeDef       DMA_InitStructure;
	  GPIO_InitTypeDef      GPIO_InitStructure;

	  // Enable peripheral clocks
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	  // DMA2_Stream0 channel0 configuration
	  DMA_DeInit(DMA2_Stream0);
	  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
	  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCConvertedValue;
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	  DMA_InitStructure.DMA_BufferSize = 4;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	  // DMA2_Stream0 enable
	  DMA_Cmd(DMA2_Stream0, ENABLE);

	  // Configure ADC1 Channel0..7 pin as analog input
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  // ADC Common Init
	  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; //21MHz ADC clock
	  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	  ADC_CommonInit(&ADC_CommonInitStructure);

	  // ADC1 Init
	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_NbrOfConversion = 4;
	  ADC_Init(ADC1, &ADC_InitStructure);



	  // ADC1 regular channel configuration
	  //15Cycles + 12 + 5 = 32 * 1/21MHz = 1.5µs
	  ADC_RegularChannelConfig(ADC1, ADC_CHAN_IOUT, 1, ADC_SampleTime_15Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_CHAN_VSM , 2, ADC_SampleTime_15Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_CHAN_VOUT, 3, ADC_SampleTime_15Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_CHAN_VIN , 4, ADC_SampleTime_15Cycles);

	  // Enable DMA request after last transfer (Single-ADC mode) */
	  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	  // Enable ADC1 DMA
	  ADC_DMACmd(ADC1, ENABLE);

	  // Enable ADC1
	  ADC_Cmd(ADC1, ENABLE);

	  // And start it
	  adc_start_conv();
}

/**
 * Start the Analog to digital conversion
 */
void adc_start_conv(void) {

	// Start ADC1 Software Conversion
	ADC_SoftwareStartConv(ADC1);

}

/**
 * Get the Result of the analog conversion
 *
 * @param
 * 			chan ADC Channel
 * @return
 * 			ADC Result
 *
 */
uint16_t adc_getResult(int chan) {
	return ADCConvertedValue[chan];
}

/**
 * get all 4 channel
 */
void adc_task(void) {

	voltage_VIN   = ADCConvertedValue[ADC_CHAN_VIN];
	current_IOUT  = ADCConvertedValue[ADC_CHAN_IOUT];
}

/**
 * get ADC value of voltage in mV
 */
int adc_VoltToADC(int volt) {
	// Voltage divider is 10k:1k
	// So the gain is set to x11
	// Reference voltage is 3.3V
	// Volt = ADC / 4096 * 3.3V * 11
	// ADC = Vout / 11 / 3.3V * 4096
	// ADC = Vout / 11 / 3.3V * 4096
	// ADC = Vout(V) * 112,837465565
	// ADC = Vout(mV) * 0.112837465565
	// DAC = ca. Vout(mV) * 231 / (2048)
	return (volt * 463) / 4096;
}

/**
 * get voltage of an ADC value in 10mV steps
 */
int adc_ADCTo10mV(int adcval) {
	// Voltage divider is 10k:1k
	// So the gain is set to x11
	// Reference voltage is 3.3V
	// Volt = ADC / 4096 * 3.3V * 11
	// Volt(10mV) = ADC / 4096 * 3.3V * 11 *100
	// Volt(10mV) = ADC * 0.886230469
	// Volt(10mV) = ADC * 1815/2048
	return (adcval * 1815) / 2048;
}

/**
 * get current of an ADC value in 1mA steps
 */
int adc_ADCTo1mA(int adcval) {
	// Offset is 3.3V/33R = 100mA
	// Voltage divider of opamp is 6.19k:1k
	// Shunt is 10mR
	// So the total gain is: 7.19*7.19*10mR = 0,516961R
	// Reference voltage is 3.3V
	// Current = ADC / 4096 * 3.3V / R
	// Current = ADC * 0.001558462
	// Current(mA) = ADC * 1.558462
	// Current(mA) = ADC * 1.558462
	int mamps = (adcval * 25) / 16 - 100;
	// limit to positive values
	if (mamps < 0)
		mamps = 0;
	return mamps;
}
