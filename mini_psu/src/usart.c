/*
 *  Project:      Mini_PSU
 *  File:         usart.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  usart bus for debugging
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
#include "usart.h"

// controller parameter
extern int32_t KP;
//extern int32_t KI;
extern int32_t KD;
extern __IO int step;

// Transmit buffer with read and write pointer
uint8_t tx_buffer[1024];
uint16_t wr_pointer = 0;
uint16_t rd_pointer = 0;

/**
 * Initialize the USART
 */
void usart_init(void) {
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* Configure USART2 Tx (PA.9) as alternate function */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);


    /* USART1 configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
          - USART Clock disabled
          - USART CPOL: Clock is active low
          - USART CPHA: Data is captured on the middle
          - USART LastBit: The clock pulse of the last data bit is not output to
                           the SCLK pin
    */
    USART_InitStructure.USART_BaudRate            = 115200;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);


}

/**
 * Handle the USART buffer
 */
void usart_buffertask(void) {
char c;

	// Send new bytes from the transmit buffer
	if (wr_pointer != rd_pointer) {
		rd_pointer++;
		rd_pointer &= 1;//0x03FF;
		while (!(USART1->SR & USART_FLAG_TXE));
		USART1->DR = 'B';//tx_buffer[rd_pointer];

	}

	// Decode the received characters
	// and use them to set the controller parameter
	if ((USART1->SR & USART_FLAG_RXNE))
	{
		c = USART1->DR & 0x00FF;
	    switch (c) {
	    case 'q':
	    	KP +=5;
	    	break;
	    case 'a':
	    		KP -=5;
	    	break;
//	    case 'w':
//	    	KI +=5;
//	    	break;
//	    case 's':
//	    	if (KI >= 5) KI -=5;
//	    	break;
	    case 'e':
	    	KD +=5;
	    	break;
	    case 'd':
	    		KD -=5;
	    	break;
	    case 'r':
	    	step +=5;
	    	break;
	    case 'f':
	    	if (step >= 5) step -=5;
	    	break;
	    }
	  }


}

/**
 * Write a new character into the transmit buffer
 * This function is called by the printf function
 *
 * @param
 * 		ch New character to send
 */
void usart_putchar(int ch) {

	// Write the character into the buffer
	wr_pointer++;
	wr_pointer &= 0x03FF;
	if (wr_pointer == rd_pointer) {
		wr_pointer--;
		wr_pointer &= 0x03FF;
	}
	tx_buffer[wr_pointer] = (ch & 0xFF);


}
