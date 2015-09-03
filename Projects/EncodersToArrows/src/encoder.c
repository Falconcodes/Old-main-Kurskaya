#include "encoder.h"

void Encoder_1_Init(void)
{
  GPIO_InitTypeDef         GPIO_InitStructure;

  /* TIM clock enable */
  ENCODER_1_TIM_CLOCK_CMD(ENCODER_1_TIM_RCC, ENABLE);

  /* GPIO clock enable */
  RCC_AHBPeriphClockCmd(ENCODER_1_GPIO_1_RCC, ENABLE);
  RCC_AHBPeriphClockCmd(ENCODER_1_GPIO_2_RCC, ENABLE);

  /* TIM channels configuration */
  GPIO_InitStructure.GPIO_Pin   = ENCODER_1_PIN_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
  GPIO_Init(ENCODER_1_GPIO_1, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = ENCODER_1_PIN_2;
  GPIO_Init(ENCODER_1_GPIO_2, &GPIO_InitStructure);

  /* Connect TIM pin to AF */
  GPIO_PinAFConfig(ENCODER_1_GPIO_1, ENCODER_1_PINSOURCE_1,  ENCODER_1_GPIO_AF);
  GPIO_PinAFConfig(ENCODER_1_GPIO_2, ENCODER_1_PINSOURCE_2,  ENCODER_1_GPIO_AF);

  /* Configure encoder */
  TIM_EncoderInterfaceConfig(ENCODER_1_TIM, TIM_EncoderMode_TI12,
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_SetAutoreload(ENCODER_1_TIM, 0xffff);

  /* Turn on */
  TIM_Cmd(ENCODER_1_TIM, ENABLE);

  /* Reset */
  TIM_SetCounter(ENCODER_1_TIM, 0x00);
}

void Encoder_2_Init(void)
{
  GPIO_InitTypeDef         GPIO_InitStructure;

  /* TIM clock enable */
  ENCODER_2_TIM_CLOCK_CMD(ENCODER_2_TIM_RCC, ENABLE);

  /* GPIO clock enable */
  RCC_AHBPeriphClockCmd(ENCODER_2_GPIO_1_RCC, ENABLE);
  RCC_AHBPeriphClockCmd(ENCODER_2_GPIO_2_RCC, ENABLE);

  /* TIM channels configuration */
  GPIO_InitStructure.GPIO_Pin   = ENCODER_2_PIN_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
  GPIO_Init(ENCODER_2_GPIO_1, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = ENCODER_2_PIN_2;
  GPIO_Init(ENCODER_2_GPIO_2, &GPIO_InitStructure);

  /* Connect TIM pin to AF */
  GPIO_PinAFConfig(ENCODER_2_GPIO_1, ENCODER_2_PINSOURCE_1,  ENCODER_2_GPIO_AF);
  GPIO_PinAFConfig(ENCODER_2_GPIO_2, ENCODER_2_PINSOURCE_2,  ENCODER_2_GPIO_AF);

  /* Configure encoder */
  TIM_EncoderInterfaceConfig(ENCODER_2_TIM, TIM_EncoderMode_TI12,
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_SetAutoreload(ENCODER_2_TIM, 0xffff);

  /* Turn on */
  TIM_Cmd(ENCODER_2_TIM, ENABLE);

  /* Reset */
  TIM_SetCounter(ENCODER_2_TIM, 0x00);
}

void Encoder_3_Init(void)
{
  GPIO_InitTypeDef         GPIO_InitStructure;

  /* TIM clock enable */
  ENCODER_3_TIM_CLOCK_CMD(ENCODER_3_TIM_RCC, ENABLE);

  /* GPIO clock enable */
  RCC_AHBPeriphClockCmd(ENCODER_3_GPIO_1_RCC, ENABLE);
  RCC_AHBPeriphClockCmd(ENCODER_3_GPIO_2_RCC, ENABLE);

  /* TIM channels configuration */
  GPIO_InitStructure.GPIO_Pin   = ENCODER_3_PIN_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
  GPIO_Init(ENCODER_3_GPIO_1, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = ENCODER_3_PIN_2;
  GPIO_Init(ENCODER_3_GPIO_2, &GPIO_InitStructure);

  /* Connect TIM pin to AF */
  GPIO_PinAFConfig(ENCODER_3_GPIO_1, ENCODER_3_PINSOURCE_1,  ENCODER_3_GPIO_AF);
  GPIO_PinAFConfig(ENCODER_3_GPIO_2, ENCODER_3_PINSOURCE_2,  ENCODER_3_GPIO_AF);

  /* Configure encoder */
  TIM_EncoderInterfaceConfig(ENCODER_3_TIM, TIM_EncoderMode_TI12,
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_SetAutoreload(ENCODER_3_TIM, 0xffff);

  /* Turn on */
  TIM_Cmd(ENCODER_3_TIM, ENABLE);

  /* Reset */
  TIM_SetCounter(ENCODER_3_TIM, 0x00);
}

int16_t Encoder_1_Get_Value()
{
  uint32_t value = TIM_GetCounter(ENCODER_1_TIM);
  return (int16_t)(value & 0xFFFF);
}

int16_t Encoder_2_Get_Value()
{
  uint32_t value = TIM_GetCounter(ENCODER_2_TIM);
  return (int16_t)(value & 0xFFFF);
}

int16_t Encoder_3_Get_Value()
{
  uint32_t value = TIM_GetCounter(ENCODER_3_TIM);
  return (int16_t)(value & 0xFFFF);
}