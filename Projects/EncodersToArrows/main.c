/**
  ******************************************************************************
  * @file    EncodersToArrows/main.c
  * @author  Yuriy (yurboc@yandex.ru)
  * @version V1.0
  * @date    24-December-2014
  * @brief   Device which used to set values of 3 voltmeters using 3 encoders.
             Actually voltmeter arrows are driven by stepper motors.
  ******************************************************************************
  */

#include "main.h"

#include "usart.h"
#include "stepper.h"
#include "encoder.h"
#include "stm32f0_discovery.h"

#define ENCODER_TO_STEPPER_RATE         64
#define ENCODER_1_RIGHT_VALUE           16 // = 4
#define ENCODER_2_RIGHT_VALUE           20 // = 5
#define ENCODER_3_RIGHT_VALUE           39 // = 10
#define ENCODER_MAX_DEVIATION           2  // +- 0.5

uint8_t checkValue(int16_t testValue, int16_t refValue)
{
  if (testValue < refValue && refValue - testValue <= ENCODER_MAX_DEVIATION)
    return 1;
  if (testValue >= refValue && testValue - refValue <= ENCODER_MAX_DEVIATION)
    return 1;

  return 0;
}

int main(void)
{
  /* Setup SysTick Timer for 3 msec interrupts. */
  if (SysTick_Config(SystemCoreClock * 3 / 35000))
    while (1);

  /* Enable board LEDs */
  STM_EVAL_LEDInit(LED3); /* PC9 */
  STM_EVAL_LEDInit(LED4); /* PC8 */

  /* Init USART on PA9(TX), PA10(RX) */
  USARTx_Init();

  /* M0(PA2,PA3,PA4,PA5); M1(PC0,PC1,PC2,PC3); M2(PC4,PC5,PC6,PC7) */
  Stepper_Init();

  /* Init encoders on TIM3(PA6,PA7), TIM1(PA8, PA9) and TIM2(PA0, PA1) */
  Encoder_1_Init();
  Encoder_2_Init();
  Encoder_3_Init();

  while (1)
  {
    Stepper_Set_Destination(0, Encoder_1_Get_Value() * ENCODER_TO_STEPPER_RATE);
    Stepper_Set_Destination(1, Encoder_2_Get_Value() * ENCODER_TO_STEPPER_RATE);
    Stepper_Set_Destination(2, Encoder_3_Get_Value() * ENCODER_TO_STEPPER_RATE);

    /* [TODO]: Check right value */
    if (checkValue(Encoder_1_Get_Value(), ENCODER_1_RIGHT_VALUE) &&
        checkValue(Encoder_2_Get_Value(), ENCODER_2_RIGHT_VALUE) &&
        checkValue(Encoder_3_Get_Value(), ENCODER_3_RIGHT_VALUE))
      STM_EVAL_LEDOn(LED3);
    else
      STM_EVAL_LEDOff(LED3);
  }
}