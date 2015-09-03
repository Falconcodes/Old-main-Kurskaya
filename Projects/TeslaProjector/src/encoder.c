#include "encoder.h"

void Encoder_Init(void)
{
  GPIO_InitTypeDef         GPIO_InitStructure;

  /* TIM clock enable */
  ENCODER_TIM_CLOCK_CMD(ENCODER_TIM_RCC, ENABLE);

  /* GPIO clock enable */
  RCC_AHBPeriphClockCmd(ENCODER_GPIO_1_RCC, ENABLE);
  RCC_AHBPeriphClockCmd(ENCODER_GPIO_2_RCC, ENABLE);

  /* TIM channels configuration */
  GPIO_InitStructure.GPIO_Pin   = ENCODER_PIN_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
  GPIO_Init(ENCODER_GPIO_1, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = ENCODER_PIN_2;
  GPIO_Init(ENCODER_GPIO_2, &GPIO_InitStructure);

  /* Connect TIM pin to AF */
  GPIO_PinAFConfig(ENCODER_GPIO_1, ENCODER_PINSOURCE_1,  ENCODER_GPIO_AF);
  GPIO_PinAFConfig(ENCODER_GPIO_2, ENCODER_PINSOURCE_2,  ENCODER_GPIO_AF);

  /* Configure encoder */
  TIM_EncoderInterfaceConfig(ENCODER_TIM, TIM_EncoderMode_TI12,
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_SetAutoreload(ENCODER_TIM, 0xffff);

  /* Turn on */
  TIM_Cmd(ENCODER_TIM, ENABLE);

  /* Reset */
  TIM_SetCounter(ENCODER_TIM, 0x00);
}

int16_t Encoder_Get_Value()
{
  uint32_t value = TIM_GetCounter(ENCODER_TIM);
  return (int16_t)(value & 0xFFFF);
}