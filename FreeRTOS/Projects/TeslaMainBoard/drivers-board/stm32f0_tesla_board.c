/* Includes ------------------------------------------------------------------*/
#include "stm32f0_tesla_board.h"
#include "stm32f0_discovery.h"
#include "CommonConfig.h"



/* STM32F0_TESLA_BOARD_LOW_LEVEL_Private_Variables */
const uint16_t RELAY_PIN[RELAYn] =      { RELAY1_PIN,
                                          RELAY2_PIN,
                                          RELAY3_PIN,
                                          RELAY4_PIN,
                                          RELAY5_PIN,
                                          RELAY6_PIN,
                                          RELAY7_PIN,
                                          RELAY8_PIN,
                                          RELAY9_PIN,
                                          RELAY10_PIN,
                                          RELAY11_PIN,
                                          RELAY12_PIN,
                                          RELAY13_PIN,
                                          RELAY14_PIN,
                                          RELAY15_PIN,
                                          RELAY16_PIN,
                                          RELAY17_PIN };

GPIO_TypeDef* RELAY_GPIO_PORT[RELAYn] = { RELAY1_GPIO_PORT,
                                          RELAY2_GPIO_PORT,
                                          RELAY3_GPIO_PORT,
                                          RELAY4_GPIO_PORT,
                                          RELAY5_GPIO_PORT,
                                          RELAY6_GPIO_PORT,
                                          RELAY7_GPIO_PORT,
                                          RELAY8_GPIO_PORT,
                                          RELAY9_GPIO_PORT,
                                          RELAY10_GPIO_PORT,
                                          RELAY11_GPIO_PORT,
                                          RELAY12_GPIO_PORT,
                                          RELAY13_GPIO_PORT,
                                          RELAY14_GPIO_PORT,
                                          RELAY15_GPIO_PORT,
                                          RELAY16_GPIO_PORT,
                                          RELAY17_GPIO_PORT };

const uint32_t RELAY_GPIO_CLK[RELAYn] = { RELAY1_GPIO_CLK,
                                          RELAY2_GPIO_CLK,
                                          RELAY3_GPIO_CLK,
                                          RELAY4_GPIO_CLK,
                                          RELAY5_GPIO_CLK,
                                          RELAY6_GPIO_CLK,
                                          RELAY7_GPIO_CLK,
                                          RELAY8_GPIO_CLK,
                                          RELAY9_GPIO_CLK,
                                          RELAY10_GPIO_CLK,
                                          RELAY11_GPIO_CLK,
                                          RELAY12_GPIO_CLK,
                                          RELAY13_GPIO_CLK,
                                          RELAY14_GPIO_CLK,
                                          RELAY15_GPIO_CLK,
                                          RELAY16_GPIO_CLK,
                                          RELAY17_GPIO_CLK };

const uint16_t LED_PIN[BOARD_LEDn] =      { BOARD_LED1_PIN,
                                            BOARD_LED2_PIN,
                                            BOARD_LED3_PIN };

GPIO_TypeDef* LED_GPIO_PORT[BOARD_LEDn] = { BOARD_LED1_GPIO_PORT,
                                            BOARD_LED2_GPIO_PORT,
                                            BOARD_LED3_GPIO_PORT };

const uint32_t LED_GPIO_CLK[BOARD_LEDn] = { BOARD_LED1_GPIO_CLK,
                                            BOARD_LED2_GPIO_CLK,
                                            BOARD_LED3_GPIO_CLK };

const uint16_t INPUT_PIN[INPUTn] =      { INPUT1_PIN,
                                          INPUT2_PIN,
                                          INPUT3_PIN,
                                          INPUT4_PIN,
                                          INPUT5_PIN,
                                          INPUT6_PIN,
                                          INPUT7_PIN,
                                          INPUT8_PIN };

GPIO_TypeDef* INPUT_GPIO_PORT[INPUTn] = { INPUT1_GPIO_PORT,
                                          INPUT2_GPIO_PORT,
                                          INPUT3_GPIO_PORT,
                                          INPUT4_GPIO_PORT,
                                          INPUT5_GPIO_PORT,
                                          INPUT6_GPIO_PORT,
                                          INPUT7_GPIO_PORT,
                                          INPUT8_GPIO_PORT };

const uint32_t INPUT_GPIO_CLK[INPUTn] = { INPUT1_GPIO_CLK,
                                          INPUT2_GPIO_CLK,
                                          INPUT3_GPIO_CLK,
                                          INPUT4_GPIO_CLK,
                                          INPUT5_GPIO_CLK,
                                          INPUT6_GPIO_CLK,
                                          INPUT7_GPIO_CLK,
                                          INPUT8_GPIO_CLK };

const uint32_t INPUT_EXTI_LINE[INPUTn] = {INPUT1_EXTI_LINE,
                                          INPUT2_EXTI_LINE,
                                          INPUT3_EXTI_LINE,
                                          INPUT4_EXTI_LINE,
                                          INPUT5_EXTI_LINE,
                                          INPUT6_EXTI_LINE,
                                          INPUT7_EXTI_LINE,
                                          INPUT8_EXTI_LINE };

const uint32_t INPUT_EXTI_PORT_SOURCE[INPUTn] = {INPUT1_EXTI_PORT_SOURCE,
                                                 INPUT2_EXTI_PORT_SOURCE,
                                                 INPUT3_EXTI_PORT_SOURCE,
                                                 INPUT4_EXTI_PORT_SOURCE,
                                                 INPUT5_EXTI_PORT_SOURCE,
                                                 INPUT6_EXTI_PORT_SOURCE,
                                                 INPUT7_EXTI_PORT_SOURCE,
                                                 INPUT8_EXTI_PORT_SOURCE };

const uint32_t INPUT_EXTI_PIN_SOURCE[INPUTn] = {INPUT1_EXTI_PIN_SOURCE,
                                                INPUT2_EXTI_PIN_SOURCE,
                                                INPUT3_EXTI_PIN_SOURCE,
                                                INPUT4_EXTI_PIN_SOURCE,
                                                INPUT5_EXTI_PIN_SOURCE,
                                                INPUT6_EXTI_PIN_SOURCE,
                                                INPUT7_EXTI_PIN_SOURCE,
                                                INPUT8_EXTI_PIN_SOURCE };

const uint32_t INPUT_EXTI_IRQn[INPUTn] = {INPUT1_EXTI_IRQn,
                                          INPUT2_EXTI_IRQn,
                                          INPUT3_EXTI_IRQn,
                                          INPUT4_EXTI_IRQn,
                                          INPUT5_EXTI_IRQn,
                                          INPUT6_EXTI_IRQn,
                                          INPUT7_EXTI_IRQn,
                                          INPUT8_EXTI_IRQn };

const uint16_t ANALOG_IN_PIN[ANALOG_INn] =         { ANALOG_IN1_PIN,
                                                     ANALOG_IN2_PIN,
                                                     ANALOG_IN3_PIN,
                                                     ANALOG_IN4_PIN,
                                                     ANALOG_IN5_PIN,
                                                     ANALOG_IN6_PIN };

GPIO_TypeDef* ANALOG_IN_GPIO_PORT[ANALOG_INn] =    { ANALOG_IN1_GPIO_PORT,
                                                     ANALOG_IN2_GPIO_PORT,
                                                     ANALOG_IN3_GPIO_PORT,
                                                     ANALOG_IN4_GPIO_PORT,
                                                     ANALOG_IN5_GPIO_PORT,
                                                     ANALOG_IN6_GPIO_PORT };

const uint32_t ANALOG_IN_GPIO_CLK[ANALOG_INn] =    { ANALOG_IN1_GPIO_CLK,
                                                     ANALOG_IN2_GPIO_CLK,
                                                     ANALOG_IN3_GPIO_CLK,
                                                     ANALOG_IN4_GPIO_CLK,
                                                     ANALOG_IN5_GPIO_CLK,
                                                     ANALOG_IN6_GPIO_CLK };

const uint32_t ANALOG_IN_ADC_CHANNEL[ANALOG_INn] = { ANALOG_IN1_ADC_CHANNEL,
                                                     ANALOG_IN2_ADC_CHANNEL,
                                                     ANALOG_IN3_ADC_CHANNEL,
                                                     ANALOG_IN4_ADC_CHANNEL,
                                                     ANALOG_IN5_ADC_CHANNEL,
                                                     ANALOG_IN6_ADC_CHANNEL };

__IO uint16_t ANALOG_IN_BUFFER[ANALOG_INn];


const uint16_t PWM_OUT_PIN[PWM_OUTn] =      { PWM_OUT1_PIN };

GPIO_TypeDef* PWM_OUT_GPIO_PORT[PWM_OUTn] = { PWM_OUT1_GPIO_PORT };

const uint32_t PWM_OUT_GPIO_CLK[PWM_OUTn] = { PWM_OUT1_GPIO_CLK };

const uint16_t PWM_OUT_SOURCE[PWM_OUTn] =   { PWM_OUT1_SOURCE };

const uint16_t PWM_OUT_AF[PWM_OUTn] =       { PWM_OUT1_AF };

TIM_TypeDef* PWM_OUT_TIM[PWM_OUTn] =        { PWM_OUT1_TIM };

const uint32_t PWM_OUT_TIM_CLK[PWM_OUTn] =  { PWM_OUT1_TIM_RCC };

TIM_OCInitTypeDef PWM_OCInitStructure[PWM_OUTn];
uint16_t PWMTimerPeriod[PWM_OUTn] = { 0 };
uint16_t PWMPulse[PWM_OUTn] =       { 0 };


TIM_TypeDef* ENCODER_TIM[ENCODERn] =            { ENCODER1_TIM,
                                                  ENCODER2_TIM,
                                                  ENCODER3_TIM };

typedef void (*ClockFuncType)(uint32_t, FunctionalState);
ClockFuncType ENCODER_TIM_CLOCK_CMD[ENCODERn] = { ENCODER1_TIM_CLOCK_CMD,
                                                  ENCODER2_TIM_CLOCK_CMD,
                                                  ENCODER3_TIM_CLOCK_CMD };

const uint32_t ENCODER_TIM_CLK[ENCODERn] =      { ENCODER1_TIM_RCC,
                                                  ENCODER2_TIM_RCC,
                                                  ENCODER3_TIM_RCC };

const uint16_t ENCODER_CH1_PIN[ENCODERn] =      { ENCODER1_CH1_PIN,
                                                  ENCODER2_CH1_PIN,
                                                  ENCODER3_CH1_PIN };

GPIO_TypeDef* ENCODER_CH1_GPIO_PORT[ENCODERn] = { ENCODER1_CH1_GPIO_PORT,
                                                  ENCODER2_CH1_GPIO_PORT,
                                                  ENCODER3_CH1_GPIO_PORT };

const uint32_t ENCODER_CH1_GPIO_CLK[ENCODERn] = { ENCODER1_CH1_GPIO_CLK,
                                                  ENCODER2_CH1_GPIO_CLK,
                                                  ENCODER3_CH1_GPIO_CLK };

const uint16_t ENCODER_CH1_SOURCE[ENCODERn] =   { ENCODER1_CH1_SOURCE,
                                                  ENCODER2_CH1_SOURCE,
                                                  ENCODER3_CH1_SOURCE };

const uint16_t ENCODER_CH1_AF[ENCODERn] =       { ENCODER1_CH1_AF,
                                                  ENCODER2_CH1_AF,
                                                  ENCODER3_CH1_AF };

const uint16_t ENCODER_CH2_PIN[ENCODERn] =      { ENCODER1_CH2_PIN,
                                                  ENCODER2_CH2_PIN,
                                                  ENCODER3_CH2_PIN };

GPIO_TypeDef* ENCODER_CH2_GPIO_PORT[ENCODERn] = { ENCODER1_CH2_GPIO_PORT,
                                                  ENCODER2_CH2_GPIO_PORT,
                                                  ENCODER3_CH2_GPIO_PORT };

const uint32_t ENCODER_CH2_GPIO_CLK[ENCODERn] = { ENCODER1_CH2_GPIO_CLK,
                                                  ENCODER2_CH2_GPIO_CLK,
                                                  ENCODER3_CH2_GPIO_CLK };

const uint16_t ENCODER_CH2_SOURCE[ENCODERn] =   { ENCODER1_CH2_SOURCE,
                                                  ENCODER2_CH2_SOURCE,
                                                  ENCODER3_CH2_SOURCE };

const uint16_t ENCODER_CH2_AF[ENCODERn] =       { ENCODER1_CH2_AF,
                                                  ENCODER2_CH2_AF,
                                                  ENCODER3_CH2_AF };

/* -------------------------------------------------------------------------- */

/* STM32F0_TESLA_BOARD_LOW_LEVEL_Private_Functions */

/**
  * @brief  Configures RELAY GPIO.
  * @param  Relay: Specifies the Relay to be configured.
  *   This parameter can be one of following parameters:
  *     @arg RELAY_IAKOV_1
  *     @arg RELAY_IAKOV_2
  *     @arg RELAY_MARKS_GEN
  *     @arg RELAY_FINAL
  * @retval None
  */
void STM_TESLA_RELAYInit(Relay_TypeDef Relay)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable the RELAY_GPIO Clock */
  RCC_AHBPeriphClockCmd(RELAY_GPIO_CLK[Relay], ENABLE);

  /* Configure the RELAY_GPIO pin */
  GPIO_InitStructure.GPIO_Pin = RELAY_PIN[Relay];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(RELAY_GPIO_PORT[Relay], &GPIO_InitStructure);
}

/**
  * @brief  Turns selected Relay On.
  * @param  Relay: Specifies the Relay to be set on.
  *   This parameter can be one of following parameters:
  *     @arg RELAY_IAKOV_1
  *     @arg RELAY_IAKOV_2
  *     @arg RELAY_MARKS_GEN
  *     @arg RELAY_FINAL
  * @retval None
  */
void STM_TESLA_RELAYOn(Relay_TypeDef Relay)
{
  RELAY_GPIO_PORT[Relay]->BSRR = RELAY_PIN[Relay];
}

/**
  * @brief  Turns selected Relay Off.
  * @param  Relay: Specifies the Relay to be set off.
  *   This parameter can be one of following parameters:
  *     @arg RELAY_IAKOV_1
  *     @arg RELAY_IAKOV_2
  *     @arg RELAY_MARKS_GEN
  *     @arg RELAY_FINAL
  * @retval None
  */
void STM_TESLA_RELAYOff(Relay_TypeDef Relay)
{
  RELAY_GPIO_PORT[Relay]->BRR = RELAY_PIN[Relay];
}

/* -------------------------------------------------------------------------- */

/**
  * @brief  Configures EXTI Line.
  * @param  Input: Specifies the Input to be configured.
  *   This parameter can be:
  *     @arg INPUT_RUBILNIK: Big switcher on the wall
  *     @arg INPUT_TUMBLERY: Box (like old refregerator) witch rotary switchers
  *     @arg INPUT_RADIOLA: Device like old radio with 3 "voltmeters"
  * @param  Input_Pull: Specifies Input pull mode.
  *   This parameter can be one of following parameters:
  *     @arg INPUT_PULL_NONE
  *     @arg INPUT_PULL_UP
  * @param  Trigger: Specifies active edge.
  *   This parameter can be one of following parameters:
  *     @arg EXTI_Trigger_Rising
  *     @arg EXTI_Trigger_Falling
  *     @arg EXTI_Trigger_Rising_Falling
  * @retval None
  */
void STM_TESLA_EXTIInit(Input_TypeDef Input, GPIOPuPd_TypeDef Input_Pull, EXTITrigger_TypeDef Trigger)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the BUTTON Clock */
  RCC_AHBPeriphClockCmd(INPUT_GPIO_CLK[Input], ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure Button pin as input */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = Input_Pull;
  GPIO_InitStructure.GPIO_Pin = INPUT_PIN[Input];
  GPIO_Init(INPUT_GPIO_PORT[Input], &GPIO_InitStructure);

  /* Connect Button EXTI Line to Button GPIO Pin */
  SYSCFG_EXTILineConfig(INPUT_EXTI_PORT_SOURCE[Input], INPUT_EXTI_PIN_SOURCE[Input]);

  /* Configure Button EXTI line */
  EXTI_InitStructure.EXTI_Line = INPUT_EXTI_LINE[Input];
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = Trigger;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set Button EXTI Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = INPUT_EXTI_IRQn[Input];
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x03;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Get EXTI Line by Input number.
  * @param  Input: Specifies the Input to be configured.
  *   This parameter can be:
  *     @arg INPUT_RUBILNIK: Big switcher on the wall
  *     @arg INPUT_TUMBLERY: Box (like old refregerator) witch rotary switchers
  *     @arg INPUT_RADIOLA: Device like old radio with 3 "voltmeters"
  * @retval EXTI Line number
  */
uint32_t STM_TESLA_EXTILine(Input_TypeDef Input)
{
  return INPUT_EXTI_LINE[Input];
}

/* -------------------------------------------------------------------------- */

/**
  * @brief  Configures Input GPIO.
  * @param  Input: Specifies the Input to be configured.
  *   This parameter can be:
  *     @arg INPUT_RUBILNIK: Big switcher on the wall
  *     @arg INPUT_TUMBLERY: Box (like old refregerator) witch rotary switchers
  *     @arg INPUT_RADIOLA: Device like old radio with 3 "voltmeters"
  * @param  Input_Pull: Specifies Input pull mode.
  *   This parameter can be one of following parameters:
  *     @arg INPUT_PULL_NONE
  *     @arg INPUT_PULL_UP
  * @retval None
  */
void STM_TESLA_INPUTInit(Input_TypeDef Input, GPIOPuPd_TypeDef Input_Pull)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable the INPUT Clock */
  RCC_AHBPeriphClockCmd(INPUT_GPIO_CLK[Input], ENABLE);

  /* Configure Button pin as input */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = Input_Pull;
  GPIO_InitStructure.GPIO_Pin = INPUT_PIN[Input];
  GPIO_Init(INPUT_GPIO_PORT[Input], &GPIO_InitStructure);
}

/**
  * @brief  Returns the selected Input state.
  * @param  Input: Specifies the Input to be checked.
  *   This parameter can be one of following parameters:
  *     @arg INPUT_RUBILNIK: Big switcher on the wall
  *     @arg INPUT_TUMBLERY: Box (like old refregerator) witch rotary switchers
  *     @arg INPUT_RADIOLA: Device like old radio with 3 "voltmeters"
  * @retval The Input GPIO pin value.
  */
uint32_t STM_TESLA_INPUTGetState(Input_TypeDef Input)
{
  /* There is no Wakeup button on STM32f0-Discovery Kit */
  return GPIO_ReadInputDataBit(INPUT_GPIO_PORT[Input], INPUT_PIN[Input]);
}

/* -------------------------------------------------------------------------- */

/**
  * @brief  Configures BUZZER GPIO.
  * @retval None
  */
void STM_TESLA_BUZZERInit()
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable the BUZZER_GPIO Clock */
  RCC_AHBPeriphClockCmd(BUZZERx_GPIO_CLK, ENABLE);

  /* Configure the RELAY_GPIO pin */
  GPIO_InitStructure.GPIO_Pin = BUZZERx_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(BUZZERx_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  Turns Buzzer On.
  * @retval None
  */
void STM_TESLA_BUZZEROn()
{
  BUZZERx_GPIO_PORT->BSRR = BUZZERx_PIN;
}

/**
  * @brief  Turns Buzzer Off.
  * @retval None
  */
void STM_TESLA_BUZZEROff()
{
  BUZZERx_GPIO_PORT->BRR = BUZZERx_PIN;
}

/* -------------------------------------------------------------------------- */

/**
  * @brief  Configures LED GPIO.
  * @param  LED: Specifies the LED to be configured.
  *   This parameter can be one of following parameters:
  *     @arg LED_RED
  *     @arg LED_GREEN
  *     @arg LED_BLUE
  * @retval None
  */
void STM_TESLA_LEDInit(LED_TypeDef LED)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable the RELAY_GPIO Clock */
  RCC_AHBPeriphClockCmd(LED_GPIO_CLK[LED], ENABLE);

  /* Configure the RELAY_GPIO pin */
  GPIO_InitStructure.GPIO_Pin = LED_PIN[LED];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED_GPIO_PORT[LED], &GPIO_InitStructure);
}

/**
  * @brief  Turns selected LED On.
  * @param  LED: Specifies the LED to be set on.
  *   This parameter can be one of following parameters:
  *     @arg LED_RED
  *     @arg LED_GREEN
  *     @arg LED_BLUE
  * @retval None
  */
void STM_TESLA_LEDOn(LED_TypeDef LED)
{
  LED_GPIO_PORT[LED]->BSRR = LED_PIN[LED];
}

/**
  * @brief  Turns selected LED Off.
  * @param  LED: Specifies the LED to be set off.
  *   This parameter can be one of following parameters:
  *     @arg LED_RED
  *     @arg LED_GREEN
  *     @arg LED_BLUE
  * @retval None
  */
void STM_TESLA_LEDOff(LED_TypeDef LED)
{
  LED_GPIO_PORT[LED]->BRR = LED_PIN[LED];
}

/* -------------------------------------------------------------------------- */

/**
  * @brief  Configures Analog Input.
  * @param  None
  * @retval None
  */
void STM_TESLA_ANALOGInit( void )
{
  GPIO_InitTypeDef    GPIO_InitStructure;
  DMA_InitTypeDef     DMA_InitStructure;
  ADC_InitTypeDef     ADC_InitStructure;

  /* GPIO Init */
  for (int ADC_Pin_Number = 0; ADC_Pin_Number < ANALOG_INn; ++ADC_Pin_Number) {
    /* GPIO Periph clock enable */
    RCC_AHBPeriphClockCmd(ANALOG_IN_GPIO_CLK[ADC_Pin_Number], ENABLE);

    /* Configure ADC Channels as analog input */
    GPIO_InitStructure.GPIO_Pin = ANALOG_IN_PIN[ADC_Pin_Number] ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(ANALOG_IN_GPIO_PORT[ADC_Pin_Number], &GPIO_InitStructure);
  }

  /* ADC DeInit */
  ADC_DeInit(ANALOG_IN_ADC);

  /* ADC Periph clock enable */
  RCC_APB2PeriphClockCmd(ANALOG_IN_ADC_CLK, ENABLE);

  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(ANALOG_IN_DMA_CLK , ENABLE);

  /* DMA Channel Config */
  DMA_DeInit(ANALOG_IN_DMA_CHANNEL);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ANALOG_IN_ADC_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)ANALOG_IN_BUFFER;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize         = ANALOG_INn;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
  DMA_Init(ANALOG_IN_DMA_CHANNEL, &DMA_InitStructure);

  /* DMA Channel enable */
  DMA_Cmd(ANALOG_IN_DMA_CHANNEL, ENABLE);

  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ANALOG_IN_ADC, ADC_DMAMode_Circular);

  /* Enable ADC_DMA */
  ADC_DMACmd(ANALOG_IN_ADC, ENABLE);

  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);

  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode   = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection        = ADC_ScanDirection_Backward; // 0-7-6-5-4-3-2-1
  ADC_Init(ANALOG_IN_ADC, &ADC_InitStructure);

  /* Convert the ADC Channels */
  for (int ADC_Pin_Number = 0; ADC_Pin_Number < ANALOG_INn; ++ADC_Pin_Number) {
    ADC_ChannelConfig(ANALOG_IN_ADC,
                      ANALOG_IN_ADC_CHANNEL[ADC_Pin_Number],
                      ANALOG_IN_ADC_ADC_SAMPLE_TIME);
  }

  /* ADC Calibration */
  ADC_GetCalibrationFactor(ANALOG_IN_ADC);

  /* Enable ADC1 */
  ADC_Cmd(ANALOG_IN_ADC, ENABLE);

  /* Wait the ADCEN falg */
  while(!ADC_GetFlagStatus(ANALOG_IN_ADC, ADC_FLAG_ADEN));

  /* ADC1 regular Software Start Conv */
  ADC_StartOfConversion(ANALOG_IN_ADC);
}

/**
  * @brief  Returns the selected Analog Input value.
  * @param  AnalogIn: Specifies the Analog Input to be readed (from buffer)
  *   This parameter can be one of following parameters:
  *     @arg ANALOG_IN1: One of phototransistors on the head
  *     @arg ANALOG_IN2: One of phototransistors on the head
  *     @arg ANALOG_IN3: One of phototransistors on the head
  *     @arg ANALOG_IN4: One of phototransistors on the head
  * @retval The Analog Input value (from last ADC -> DMA -> Memory transaction).
  */
uint16_t STM_TESLA_ANALOGGetValue( AnalogIn_TypeDef AnalogIn )
{
  return ANALOG_IN_BUFFER[AnalogIn];
}

/* -------------------------------------------------------------------------- */

/**
  * @brief  Configures PWM Output.
  * @param  PWMOut: Specifies the PWM Output to be configured.
  *   This parameter can be:
  *     @arg PWM_OUT_TO_GENERATOR: Show value on "Ampermeter" inside generator
  * @retval None
  */
void STM_TESLA_PWMInit( PWM_Out_TypeDef PWMOut )
{
  GPIO_InitTypeDef         GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  /* GPIO Clocks enable */
  RCC_AHBPeriphClockCmd( PWM_OUT_GPIO_CLK[PWMOut], ENABLE);

  /* GPIO Configuration: Channel X as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin   = PWM_OUT_PIN[PWMOut];
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
  GPIO_Init(PWM_OUT_GPIO_PORT[PWMOut], &GPIO_InitStructure);

  GPIO_PinAFConfig(PWM_OUT_GPIO_PORT[PWMOut],
                   PWM_OUT_SOURCE[PWMOut],
                   PWM_OUT_AF[PWMOut]);

  /* TIM1 Configuration */

  /* Compute the value to be set in ARR regiter
     to generate signal frequency at 10 kHz */
  PWMTimerPeriod[PWMOut] = (SystemCoreClock / 10000 ) - 1;
  /* Compute CCR1 value to generate a duty cycle at 0% (OFF) for channel 1 */
  PWMPulse[PWMOut] = (uint16_t) (((uint32_t) 0 *
                                  (PWMTimerPeriod[PWMOut] - 1)) / 100);

  /* TIM clock enable */
  RCC_APB2PeriphClockCmd(PWM_OUT_TIM_CLK[PWMOut] , ENABLE);

  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler         = 0;
  TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period            = PWMTimerPeriod[PWMOut];
  TIM_TimeBaseStructure.TIM_ClockDivision     = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(PWM_OUT_TIM[PWMOut], &TIM_TimeBaseStructure);

  /* Channel Configuration in PWM mode */
  PWM_OCInitStructure[PWMOut].TIM_OCMode       = TIM_OCMode_PWM2;
  PWM_OCInitStructure[PWMOut].TIM_OutputState  = TIM_OutputState_Enable;
  PWM_OCInitStructure[PWMOut].TIM_OutputNState = TIM_OutputNState_Disable;
  PWM_OCInitStructure[PWMOut].TIM_OCPolarity   = TIM_OCPolarity_Low;
  PWM_OCInitStructure[PWMOut].TIM_OCNPolarity  = TIM_OCNPolarity_High;
  PWM_OCInitStructure[PWMOut].TIM_OCIdleState  = TIM_OCIdleState_Set;
  PWM_OCInitStructure[PWMOut].TIM_OCNIdleState = TIM_OCIdleState_Reset;

  PWM_OCInitStructure[PWMOut].TIM_Pulse = PWMPulse[PWMOut];
  TIM_OC1Init(PWM_OUT_TIM[PWMOut], &PWM_OCInitStructure[PWMOut]);

  /* TIM1 counter enable */
  TIM_Cmd(PWM_OUT_TIM[PWMOut], ENABLE);

  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(PWM_OUT_TIM[PWMOut], ENABLE);
}

/**
  * @brief  Sets the selected PWM Output duty cycle.
  * @param  PWMOut: Specifies the PWM Output to be updated
  *   This parameter can be one of following parameters:
  *     @arg PWM_OUT_TO_GENERATOR: Show value on "Ampermeter" inside generator
  * @param  DutyCycle: Specifies PWM Duty Cycle.
  *   This parameter can be 0..100 (in percents), so 0=OFF; 100=MAX_VOLTAGE.
  * @retval None
  */
void STM_TESLA_PWMSetDutyCycle( PWM_Out_TypeDef PWMOut, uint16_t DutyCycle )
{
  PWMPulse[PWMOut] = (uint16_t) (((uint32_t) DutyCycle *
                                  (PWMTimerPeriod[PWMOut] - 1)) / 100);

  PWM_OCInitStructure[PWMOut].TIM_Pulse = PWMPulse[PWMOut];
  TIM_OC1Init(PWM_OUT_TIM[PWMOut], &PWM_OCInitStructure[PWMOut]);
}

/* -------------------------------------------------------------------------- */

/**
  * @brief  Configures Encoder Input.
  * @param  Encoder: Specifies the Encoder to be configured.
  *   This parameter can be:
  *     @arg ENCODER_FROM_GENERATOR: Detect handle rotation on the generator
  * @retval None
  */
void STM_TESLA_ENCODERInit( Encoder_TypeDef Encoder )
{
  GPIO_InitTypeDef         GPIO_InitStructure;

  /* TIM clock enable */
  ClockFuncType TIM_StartClock = ENCODER_TIM_CLOCK_CMD[Encoder];
  TIM_StartClock(ENCODER_TIM_CLK[Encoder], ENABLE);

  /* GPIO clock enable */
  RCC_AHBPeriphClockCmd(ENCODER_CH1_GPIO_CLK[Encoder], ENABLE);
  RCC_AHBPeriphClockCmd(ENCODER_CH2_GPIO_CLK[Encoder], ENABLE);

  /* TIM channels configuration */
  GPIO_InitStructure.GPIO_Pin   = ENCODER_CH1_PIN[Encoder];
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
  GPIO_Init(ENCODER_CH1_GPIO_PORT[Encoder], &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = ENCODER_CH2_PIN[Encoder];
  GPIO_Init(ENCODER_CH2_GPIO_PORT[Encoder], &GPIO_InitStructure);

  /* Connect TIM pin to AF */
  GPIO_PinAFConfig(ENCODER_CH1_GPIO_PORT[Encoder],
                   ENCODER_CH1_SOURCE[Encoder],
                   ENCODER_CH1_AF[Encoder]);
  GPIO_PinAFConfig(ENCODER_CH2_GPIO_PORT[Encoder],
                   ENCODER_CH2_SOURCE[Encoder],
                   ENCODER_CH2_AF[Encoder]);

  /* Configure encoder */
  TIM_EncoderInterfaceConfig(ENCODER_TIM[Encoder],
                             TIM_EncoderMode_TI12,
                             TIM_ICPolarity_Rising,
                             TIM_ICPolarity_Rising);
  TIM_SetAutoreload(ENCODER_TIM[Encoder], 0xffff);

  /* Turn on */
  TIM_Cmd(ENCODER_TIM[Encoder], ENABLE);

  /* Reset */
  TIM_SetCounter(ENCODER_TIM[Encoder], 0x00);
}

/**
  * @brief  Returns the selected Encoder Input value.
  * @param  Encoder: Specifies the Encoder to be readed
  *   This parameter can be one of following parameters:
  *     @arg ENCODER_FROM_GENERATOR: Detect handle rotation on the generator
  * @retval The Encoder current value.
  */
int16_t STM_TESLA_ENCODERGetValue( Encoder_TypeDef Encoder )
{
  uint32_t value = TIM_GetCounter(ENCODER_TIM[Encoder]);
  return (int16_t)(value & 0xFFFF);
}

/* -------------------------------------------------------------------------- */

/**
  * @brief  Initializes all board hardware.
  * @param  None
  * @retval None
  */
void vBoardHardwareInitialize( void )
{
  /* Ininialize USART interface */
  vSerialPortInit(serCOM1, USARTx_BAUD, USARTx_BUFFER_LEN);
  vSerialPortInit(serCOM2, USARTy_BAUD, USARTy_BUFFER_LEN);

  /* Initialize I2C interface */
  vI2CPortInitMinimal();

  /* Initialize all relays */
  STM_TESLA_RELAYInit((Relay_TypeDef)0);
  STM_TESLA_RELAYInit((Relay_TypeDef)1);
  STM_TESLA_RELAYInit((Relay_TypeDef)2);
  STM_TESLA_RELAYInit((Relay_TypeDef)3);
  STM_TESLA_RELAYInit((Relay_TypeDef)4);
  STM_TESLA_RELAYInit((Relay_TypeDef)5);
  STM_TESLA_RELAYInit((Relay_TypeDef)6);
  STM_TESLA_RELAYInit((Relay_TypeDef)7);

  STM_TESLA_RELAYInit((Relay_TypeDef)8);
  STM_TESLA_RELAYInit((Relay_TypeDef)9);
  STM_TESLA_RELAYInit((Relay_TypeDef)10);
  STM_TESLA_RELAYInit((Relay_TypeDef)11);
  STM_TESLA_RELAYInit((Relay_TypeDef)12);
  STM_TESLA_RELAYInit((Relay_TypeDef)13);
  STM_TESLA_RELAYInit((Relay_TypeDef)14);
  STM_TESLA_RELAYInit((Relay_TypeDef)15);

  STM_TESLA_RELAYInit((Relay_TypeDef)16);

  /* Initialize digital inputs */
  STM_TESLA_INPUTInit(INPUT_RUBILNIK, GPIO_PuPd_UP);
  STM_TESLA_INPUTInit(INPUT_TUMBLERY, GPIO_PuPd_UP);
  STM_TESLA_INPUTInit(INPUT_RADIOLA,  GPIO_PuPd_NOPULL);
  STM_TESLA_INPUTInit(INPUT_KEYHOLE,  GPIO_PuPd_UP);
  STM_TESLA_INPUTInit(INPUT_GRAPHITE, GPIO_PuPd_UP);
  STM_TESLA_INPUTInit(INPUT_5, GPIO_PuPd_UP);
#ifdef ENABLE_ELECTROMECH
  STM_TESLA_EXTIInit (INPUT_7X7KNOPKI,  GPIO_PuPd_DOWN, EXTI_Trigger_Rising);
  STM_TESLA_EXTIInit (INPUT_BELL,       GPIO_PuPd_UP,   EXTI_Trigger_Falling);
  STM_TESLA_EXTIInit (INPUT_220V_SENSE, GPIO_PuPd_UP,   EXTI_Trigger_Rising);
#endif


  /* Initialize buzzer */
  STM_TESLA_BUZZERInit();

  /* Initialize LEDs */
  STM_TESLA_LEDInit(LED_RED);
  STM_TESLA_LEDInit(LED_GREEN);
  STM_TESLA_LEDInit(LED_BLUE);

  /* Initialize analog inputs */
  STM_TESLA_ANALOGInit();

  /* Initialize PWM outputs */
  STM_TESLA_PWMInit(PWM_OUT_TO_GENERATOR);

  /* Initialize encoders */
  STM_TESLA_ENCODERInit(ENCODER_FROM_GENERATOR);
  STM_TESLA_ENCODERInit(ENCODER_FROM_PROJECTOR);
}