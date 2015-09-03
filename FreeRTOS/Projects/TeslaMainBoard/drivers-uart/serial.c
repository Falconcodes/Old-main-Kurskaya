/*
    FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

	***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
	***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
	the FAQ page "My application does not run, what could be wrong?".  Have you
	defined configASSERT()?

	http://www.FreeRTOS.org/support - In return for receiving this top quality
	embedded software for free we request you assist our global community by
	participating in the support forum.

	http://www.FreeRTOS.org/training - Investing in training allows your team to
	be as productive as possible as early as possible.  Now you can receive
	FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
	Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
	BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR UART0.
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Hardware specification. */
#include "stm32f0_tesla_board.h"

/* Demo application includes. */
#include "serial.h"
#include "xprintf.h"
/*-----------------------------------------------------------*/

/* Misc defines. */
#define serINVALID_QUEUE        ( ( QueueHandle_t ) 0 )
#define serNO_BLOCK             ( ( TickType_t ) 0 )
#define serTX_BLOCK_TIME        ( 40 / portTICK_PERIOD_MS )

/*-----------------------------------------------------------*/

/* The queue used to hold received characters. */
static QueueHandle_t xRxedChars[2];
static QueueHandle_t xCharsForTx[2];

/*-----------------------------------------------------------*/

/* UART interrupt handlers. */
void vUARTxInterruptHandler( void );
void vUARTyInterruptHandler( void );

/*-----------------------------------------------------------*/

/* Send character to first USART with default TIMEOUT */
void vPutCharToUSARTx( unsigned char data )
{
  xSerialPutChar(serCOM1, data, serNO_BLOCK);
}

/* Send character to second USART with default TIMEOUT */
void vPutCharToUSARTy( unsigned char data )
{
  xSerialPutChar(serCOM2, data, serNO_BLOCK);
}

/*-----------------------------------------------------------*/

/*
 * See the serial.h header file.
 */
void vSerialPortInit( eCOMPort ePort, unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Create the queues used to hold Rx/Tx characters. */
  xRxedChars[ePort] = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
  xCharsForTx[ePort] = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );

  /* If the queue/semaphore was created correctly then setup the serial port hardware. */
  if( ( xRxedChars[ePort] == serINVALID_QUEUE ) || ( xCharsForTx[ePort] == serINVALID_QUEUE ) )
    return;

  if (ePort == serCOM1) {
    /* Enable USART1 and GPIO clocks */
    USARTx_APBPERIPHCLOCK( USARTx_CLK, ENABLE );
    RCC_AHBPeriphClockCmd( USARTx_TX_GPIO_CLK, ENABLE );
    RCC_AHBPeriphClockCmd( USARTx_RX_GPIO_CLK, ENABLE );

    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);

    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);
  }
  else {
    /* Enable USART2 and GPIO clocks */
    USARTy_APBPERIPHCLOCK( USARTy_CLK, ENABLE );
    RCC_AHBPeriphClockCmd( USARTy_TX_GPIO_CLK, ENABLE );
    RCC_AHBPeriphClockCmd( USARTy_RX_GPIO_CLK, ENABLE );

    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(USARTy_TX_GPIO_PORT, USARTy_TX_SOURCE, USARTy_TX_AF);

    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(USARTy_RX_GPIO_PORT, USARTy_RX_SOURCE, USARTy_RX_AF);
  }

  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  if (ePort == serCOM1) {
    GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
    GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;
    GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);
  }
  else {
    GPIO_InitStructure.GPIO_Pin = USARTy_TX_PIN;
    GPIO_Init(USARTy_TX_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = USARTy_RX_PIN;
    GPIO_Init(USARTy_RX_GPIO_PORT, &GPIO_InitStructure);
  }

  /* Configure USART parameters */
  USART_InitStructure.USART_BaudRate            = ulWantedBaud;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init((ePort == serCOM1) ? USARTx : USARTy, &USART_InitStructure);

  /* Configure interrupts controller */
  NVIC_InitStructure.NVIC_IRQChannel = (ePort == serCOM1) ? USARTx_IRQChannel
                                                          : USARTy_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &NVIC_InitStructure );

  USART_ITConfig((ePort == serCOM1) ? USARTx : USARTy, USART_IT_RXNE, ENABLE);

  /* Enable the USART */
  USART_Cmd((ePort == serCOM1) ? USARTx : USARTy, ENABLE);

  /* Init the xprintf library on debug interface */
  if (ePort == serCOM1)
    xdev_out(vPutCharToUSARTx);
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( eCOMPort ePort, signed char *pcRxedChar, TickType_t xBlockTime )
{
  /* Get the next character from the buffer.  Return false if no characters
  are available, or arrive before xBlockTime expires. */
  if( xQueueReceive( xRxedChars[ePort], pcRxedChar, xBlockTime ) )
  {
    return pdTRUE;
  }
  else
  {
    return pdFALSE;
  }
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( eCOMPort ePort, signed char cOutChar, TickType_t xBlockTime )
{
  signed portBASE_TYPE xReturn;

  if( xQueueSend( xCharsForTx[ePort], &cOutChar, xBlockTime ) == pdPASS )
  {
    xReturn = pdPASS;
    USART_ITConfig((ePort == serCOM1) ? USARTx : USARTy, USART_IT_TXE, ENABLE);
  }
  else
  {
    xReturn = pdFAIL;
  }

  return xReturn;
}
/*-----------------------------------------------------------*/

void vUARTxInterruptHandler( void )
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
  char cChar;

  if( USART_GetITStatus( USARTx, USART_IT_TXE ) == SET )
  {
    /* The interrupt was caused by the THR becoming empty.  Are there any
    more characters to transmit? */
    if( xQueueReceiveFromISR( xCharsForTx[serCOM1], &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
    {
      /* A character was retrieved from the queue so can be sent to the THR now. */
      USART_SendData( USARTx, cChar );
    }
    else
    {
      USART_ITConfig( USARTx, USART_IT_TXE, DISABLE );
    }
  }

  if( USART_GetITStatus( USARTx, USART_IT_RXNE ) == SET )
  {
    cChar = USART_ReceiveData( USARTx );
    xQueueSendFromISR( xRxedChars[serCOM1], &cChar, &xHigherPriorityTaskWoken );
  }

  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void vUARTyInterruptHandler( void )
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
  char cChar;

  if( USART_GetITStatus( USARTy, USART_IT_TXE ) == SET )
  {
    /* The interrupt was caused by the THR becoming empty.  Are there any
    more characters to transmit? */
    if( xQueueReceiveFromISR( xCharsForTx[serCOM2], &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
    {
      /* A character was retrieved from the queue so can be sent to the THR now. */
      USART_SendData( USARTy, cChar );
    }
    else
    {
      USART_ITConfig( USARTy, USART_IT_TXE, DISABLE );
    }
  }

  if( USART_GetITStatus( USARTy, USART_IT_RXNE ) == SET )
  {
    cChar = USART_ReceiveData( USARTy );
    xQueueSendFromISR( xRxedChars[serCOM2], &cChar, &xHigherPriorityTaskWoken );
  }

  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}