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

// SPI state
int spi_tcnt = 0;
// command
int CmdStatus = 0x00;
// Bytes to send
uint8_t spi_txdata[2] = {0,0};
// Result of the altimeter
uint32_t d1_result = 0;
// SPI result
uint8_t spi_result = 0x00;
// The gyro signal
int spi_gyro = 0;
// The received SPI data
uint8_t spi_rxdata[4] = {0,0,0,0};
// The gyro data
uint8_t spi_gyrodata[6] = {0,0,0,0,0,0};
// Bytes to send and to receive
int spi_tx_bytes = 0;
int spi_rx_bytes = 0;
// Counter to read out the gyro registers
int gyro_i;

// Configuration structure of the gyro module
#define GYRO_CONFS 6
uint8_t gyro_conf[GYRO_CONFS][2] = {
	{LSM330DL_CTRL_REG1_G,0xFF}, // 800Hz/ 110Hz X,Y,Z enabled
	{LSM330DL_CTRL_REG2_G,0x00}, // no HighPass Filter
	{LSM330DL_CTRL_REG3_G,0x00}, // no IRQ
	{LSM330DL_CTRL_REG4_G,0x20}, // blockDataUpdate +-2000dps
	{LSM330DL_CTRL_REG5_G,0x00}, // no FIFO
	{LSM330DL_DATACAPTURE_G,0x00}// no IRQ
	};//


/**
 * Initialize the SPI
 */
void spi_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// Enable the SPI clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	// Enable GPIO clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	// SPI GPIO Configuration
	// Connect SPI pins to AF5
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	// SPI SCK pin configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// SPI MISO pin configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// SPI  MOSI pin configuration
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// SPI configuration -------------------------------------------------------
	SPI_I2S_DeInit(SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPI2, &SPI_InitStructure);

	// Configure the Priority Group to 1 bit
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// Configure the SPI interrupt priority
	NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	// Enable GPIO clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	/* Configure PE5,6,8 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//Enable the Rx buffer not empty interrupt */
	//SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);

	SPI_CS_ALT_DISABLE();
	SPI_CS_GYRO_DISABLE();
	SPI_CS_ACC_DISABLE();

	spi_tcnt = 0;

	// The Data transfer is performed in the SPI interrupt routine
	// Enable the SPI peripheral
	SPI_Cmd(SPI2, ENABLE);

}

/**
 * Interrupt callback function for the received bytes
 */
void spi_rx_irq(void) {

	// Receive the byte and store it in a buffer
    if (spi_rx_bytes>0) {
        spi_rx_bytes--;
    	spi_rxdata[spi_tx_bytes] = SPI_I2S_ReceiveData(SPI2);
    }
}

/**
 * Interrupt service function for the transmittion of a new byte
 */
void spi_tx_irq(void) {

	// Was ist the first byte, the command ?
	if (CmdStatus == 0)
    {
	    // Send Transaction code
      SPI_I2S_SendData(SPI2, spi_txdata[0]);
      CmdStatus = 1;
    }
    else
    {
    	// Receive the bytes
        if (spi_rx_bytes>0) {
            spi_rx_bytes--;
        	spi_rxdata[spi_rx_bytes] = SPI_I2S_ReceiveData(SPI2);
        }

        // Send the next byte ..
    	spi_tx_bytes--;
    	if (spi_tx_bytes >0) {
    		SPI_I2S_SendData(SPI2,spi_txdata[1]);
    	}
    	//.. or stop transmitting new bytes
    	else {
            SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
    	}
    }

}

/**
 * Send maximum 2 bytes and receive n bytes
 *
 * @param
 * 			bytes Number of bytes to send and to receive
 * 			txdata1 First byte to send
 * 			txdata2 2nd byte to send
 *
 */
void spi_send(int bytes, uint8_t txdata1,uint8_t txdata2 ) {
	spi_tx_bytes = bytes;
	spi_rx_bytes = bytes;
	spi_txdata[0] = txdata1;
	spi_txdata[1] = txdata2;
	CmdStatus = 0;
	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);
}

/**
 * The SPI state machine
 */
void spi_machine(void) {

    switch (spi_tcnt) {
    /*
    case 1:
    	SPI_CS_ALT_ENABLE();
	    spi_send(1,0,MS5611_CMD_RESET);
        break;

    case 92:
    	SPI_CS_ALT_DISABLE();
        break;

    case 100:
    	SPI_CS_ALT_ENABLE();
	    spi_send(1,0,MS5611_CMD_CONV_D1_4096);
        break;

    case 192:
    	SPI_CS_ALT_DISABLE();
        break;

    case 193:
    	SPI_CS_ALT_ENABLE();
	    spi_send(4,4,MS5611_CMD_ADC_READ);
        break;

    case 194:
    	d1_result = (uint32_t)spi_rxdata[2]<<16 | (uint32_t)spi_rxdata[1]<<8  |(uint32_t)spi_rxdata[0];
    	SPI_CS_ALT_DISABLE();
        break;
*/

    // Initialize the gyro
    // Read the REG1
    case 1:
    	SPI_CS_GYRO_ENABLE();
	    spi_send(2,LSM330DL_CTRL_REG1_G | LSM330DL_READ,0);
        break;

    case 2:
    	spi_result = spi_rxdata[0];
    	SPI_CS_GYRO_DISABLE();
    	gyro_i = 0;
        break;

    // Send the configuration structure to the gyro
    case 3:
    	if (gyro_i < GYRO_CONFS) {
        	SPI_CS_GYRO_ENABLE();
    	    spi_send(2,gyro_conf[gyro_i][0] | LSM330DL_WRITE, gyro_conf[gyro_i][1]);
    	    gyro_i++;
    	}
    	else {
    		gyro_i = 0;
    		spi_tcnt = 5-1;
    	}
        break;

    case 4:
    	SPI_CS_GYRO_DISABLE();
		spi_tcnt = 3-1;
        break;

    // At case 100, the cyclic procedure starts

    // Read the gyro status
    case 100:
    	SPI_CS_GYRO_ENABLE();
	    spi_send(2,LSM330DL_STATUS_REG_G | LSM330DL_READ,0);
        break;

    // Is there new Z-data available ?
    case 101:
    	SPI_CS_GYRO_DISABLE();
    	if (spi_rxdata[0] & 0x04) { // new Z-Data available ?;
    		// Continue with case 102
    		spi_tcnt = 102-1;
    	}
    	else {
    		// Go back to case 100
    		spi_tcnt = 100-1;
    	}
        break;

    // Read out the X,Y and Z-Data
    case 102:
    	SPI_CS_GYRO_DISABLE();
    	if (gyro_i> 0) {
        	spi_gyrodata[gyro_i-1] = spi_rxdata[0];
    	}
    	if (gyro_i < 6) {
        	SPI_CS_GYRO_ENABLE();
    	    spi_send(2,(LSM330DL_OUT_X_L_G+gyro_i) | LSM330DL_READ,0);
        	gyro_i++;
    		spi_tcnt = 102-1;
    	}
    	else {
        	SPI_CS_GYRO_DISABLE();
        	// Get the Z-Data
        	spi_gyro = (int)(int16_t)((uint16_t)spi_gyrodata[5] << 8 | spi_gyrodata[4]);
    		gyro_i = 0;
    		spi_tcnt = 100-1;
    	}
        break;

    // Wait some cycles until there could be new data available
    case 108:
    	spi_tcnt = 100-1;
        break;
    }
    spi_tcnt++;
}

/**
 * Getter for the gyro data
 *
 * @return
 * 			The gyro rate
 */
int spi_getGyro(void) {
	return spi_gyro;
}

/**
 * Getter for the altimeter
 *
 * @return
 * 		The air pressure
 */
uint32_t spi_get_d1(void) {
	// 1000m = ca 100mbar
	// 1m = 0.1mbar
	// 10cm = 0.01mbar
	// 83 264 96
	// 83 267 78
	// 83 253 68
	// 83 258 34
	return d1_result;
}
