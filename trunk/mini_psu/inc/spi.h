/*
 *  Project:      Mini_PSU
 *  File:         spi.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for spi.c
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
#ifndef SPI_H_
#define SPI_H_
#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "misc.h"

// Function Prototypes
void spi_init(void);
void spi_tx_irq(void);
void spi_rx_irq(void);
void spi_machine(void);
void spi_send(int bytes,uint8_t txdata1,uint8_t txdata2 );
int spi_getGyro(void);

// Macros
#define SPI_CS_ALT_DISABLE() GPIOE->BSRRL = GPIO_Pin_8;
#define SPI_CS_ALT_ENABLE() GPIOE->BSRRH = GPIO_Pin_8;
#define SPI_CS_GYRO_DISABLE() GPIOE->BSRRL = GPIO_Pin_5;
#define SPI_CS_GYRO_ENABLE() GPIOE->BSRRH = GPIO_Pin_5;
#define SPI_CS_ACC_DISABLE() GPIOE->BSRRL = GPIO_Pin_6;
#define SPI_CS_ACC_ENABLE() GPIOE->BSRRH = GPIO_Pin_6;


// Constant values
#define MS5611_CMD_ADC_READ 0x00
#define MS5611_CMD_RESET 0x1E
#define MS5611_CMD_CONV_D1_256 0x40
#define MS5611_CMD_CONV_D1_512 0x42
#define MS5611_CMD_CONV_D1_1024 0x44
#define MS5611_CMD_CONV_D1_2048 0x46
#define MS5611_CMD_CONV_D1_4096 0x48
#define MS5611_CMD_CONV_D2_256 0x50
#define MS5611_CMD_CONV_D2_512 0x52
#define MS5611_CMD_CONV_D2_1024 0x54
#define MS5611_CMD_CONV_D2_2048 0x56
#define MS5611_CMD_CONV_D2_4096 0x58


#define LSM330DL_CTRL_REG1_G   0x20
#define LSM330DL_CTRL_REG2_G   0x21
#define LSM330DL_CTRL_REG3_G   0x22
#define LSM330DL_CTRL_REG4_G   0x23
#define LSM330DL_CTRL_REG5_G   0x24
#define LSM330DL_DATACAPTURE_G 0x25
#define LSM330DL_OUT_TEMP_G    0x26
#define LSM330DL_STATUS_REG_G  0x27
#define LSM330DL_OUT_X_L_G     0x28
#define LSM330DL_OUT_X_H_G     0x29
#define LSM330DL_OUT_Y_L_G     0x2A
#define LSM330DL_OUT_Y_H_G     0x2B
#define LSM330DL_OUT_Z_L_G     0x2C
#define LSM330DL_OUT_Z_H_G     0x2D

#define LSM330DL_READ 0x80
#define LSM330DL_WRITE 0x00


#endif /* SPI_H_ */
