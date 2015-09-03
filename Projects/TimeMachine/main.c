/**
  ******************************************************************************
  * @file    TimeMachine/main.c
  * @author  Yuriy (yurboc@yandex.ru)
  * @version V1.0
  * @date    23-November-2014
  * @brief   Device which used to input date in the "Time Machine".
             In this version month and year specifies using variable resistors.
  ******************************************************************************
  */

/*
USART message, 1 byte: [ PPPP DDDD ]
    P - position, 0..7
    D - digit, 0..9; special value 15 - turn off digit
*/

#include "main.h"

#include "nixie_driver.h"
#include "usart.h"
#include "xprintf.h"

#define DIGITS_COUNT 8

uint8_t  displayedDigits[DIGITS_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0 };

uint32_t displayValue  = 0; /* Shown value: from 00000000 to 99999999 */
uint8_t  receivedByte  = 0; /* Value which read from USART */

uint32_t calculateDisplayValue(uint8_t digit, uint8_t pos)
{
  /* Check input data */
  if (pos >= DIGITS_COUNT || digit > 9)
    return 0;

  /* Update value */
  displayedDigits[pos] = digit;

  /* Calculate new value */
  uint32_t res = 0;
  uint32_t mul = 1;
  for (int i = 0; i < DIGITS_COUNT; ++i) {
    res += displayedDigits[i] * mul;
    mul *= 10;
  }

  return res;
}

int main(void)
{
  /* Setup SysTick Timer for 1 msec interrupts. */
  if (SysTick_Config(SystemCoreClock / 1000))
    while (1);

  /* Init NIXIE bulbs on: PA0,1,2,3; PA4,5,6,7 (anodes);
                          PF0,PF1,PA13,PA14 (cathode multipl.) */
  NIXIE_Init();

  /* Init USART on PA9(TX), PA10(RX) */
  USARTx_Init();

  /* Wait for message from USART */
  USARTx_Prepare_Receiver(&receivedByte, /* size = */ 1);

  /* Send "ready" after start */
  Delay(1000);
  xprintf("TIME MACHINE READY\n");

  while (1)
  {
    /* Check for new data on USART */
    if (USARTx_Is_Receive_Complete()) {
      uint8_t pos = (receivedByte & 0xF0) >> 4;
      uint8_t digit = (receivedByte & 0xF);
      displayValue = calculateDisplayValue(digit, pos);

      NIXIE_WriteDigit(digit, pos);
      //NIXIE_WriteValue(displayValue);

      /* Restart USART DMA */
      USARTx_Prepare_Receiver(&receivedByte, /* size = */ 1);
    }

    /* Update display each 1 ms */
    NIXIE_NextTick();
    Delay(1);
  }
}