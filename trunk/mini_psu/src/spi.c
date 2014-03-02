/*
 *  Project:      Mini_PSU
 *  File:         spi.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  SPI bus to the gyro and pressure sensor
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

#include "spi.h"
SPI_InitTypeDef  SPI_InitStructure;

/**
 * Initialize the SPI
 */
void spi_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable the SPI clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	// Enable GPIO clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	// SPI GPIO Configuration
	// Connect SPI pins to AF5
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2);
	//GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	// SPI SCK pin configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// SPI  MOSI pin configuration
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// SPI CS pin configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	// SPI configuration -------------------------------------------------------
	SPI_I2S_DeInit(SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // ca. 5MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_CS_DISABLE();

	// Enable the SPI peripheral
	SPI_Cmd(SPI2, ENABLE);

}

/**
 * Send one byte
 *
 * @param
 * 			byte to send
 *
 */
void spi_send( uint8_t byte ) {
	volatile int i;
	// Wait until ready to transmit
	//while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));

	// activate the CS  line
	SPI_CS_ENABLE();

	// Short delay
	for (i = 0; i < 20; i++);

	// Send one byte
	SPI_I2S_SendData(SPI2, byte);

	// Short delay
	for (i = 0; i < 50; i++);

	SPI_CS_DISABLE();
}
