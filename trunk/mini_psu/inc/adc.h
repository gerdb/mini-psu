/*
 *  Project:      Mini_PSU
 *  File:         adc.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for adc.c
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
#ifndef ADC_H_
#define ADC_H_

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_adc.h"

// Function Prototypes
void adc_init();
void adc_start_conv(void);
uint16_t adc_getResult(int chan);

// Constant values
#define ADC_CHAN_IOUT 0
#define ADC_CHAN_VSSM 1
#define ADC_CHAN_VOUTM 2
#define ADC_CHAN_VIN 3

#define ADC1_DR_ADDRESS    ((uint32_t)0x4001204C)

#endif /* ADC_H_ */
