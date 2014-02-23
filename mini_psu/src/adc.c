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
#include "project.h"

// The result of the analog to digital conversion
__IO uint16_t ADCConvertedValue[11] = {0,0,0,0,0,0,0,0,0,0,0};

/**
 * Initializes the ADC
 */
void adc_init() {
	  ADC_InitTypeDef       ADC_InitStructure;
	  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	  DMA_InitTypeDef       DMA_InitStructure;
	  GPIO_InitTypeDef      GPIO_InitStructure;

	  // Enable peripheral clocks
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	  // DMA2_Stream0 channel0 configuration
	  DMA_DeInit(DMA2_Stream0);
	  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
	  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCConvertedValue;
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	  DMA_InitStructure.DMA_BufferSize = 11;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	  // DMA2_Stream0 enable
	  DMA_Cmd(DMA2_Stream0, ENABLE);

	  // Configure ADC1 Channel0..7 pin as analog input
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  // Configure ADC1 Channel8..9 pin as analog input
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);

	  // Configure ADC1 Channel10 pin as analog input
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOC, &GPIO_InitStructure);

	  // ADC Common Init
	  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div6;
	  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	  ADC_CommonInit(&ADC_CommonInitStructure);

	  // ADC1 Init
	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	  ADC_InitStructure.ADC_ExternalTrigConv =
	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_NbrOfConversion = 11;
	  ADC_Init(ADC1, &ADC_InitStructure);

	  // Enable ADC1 DMA
	  ADC_DMACmd(ADC1, ENABLE);

	  // ADC1 regular channel18 (VBAT) configuration
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_112Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_112Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_112Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_112Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_112Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_112Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_112Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 8, ADC_SampleTime_112Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 9, ADC_SampleTime_112Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 10, ADC_SampleTime_112Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 11, ADC_SampleTime_112Cycles);

	  // Enable DMA request after last transfer (Single-ADC mode) */
	  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	  // Enable ADC1
	  ADC_Cmd(ADC1, ENABLE);
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
