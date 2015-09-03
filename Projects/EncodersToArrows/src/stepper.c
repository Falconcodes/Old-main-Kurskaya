#include "stepper.h"
#include "delay.h"

#define STEPS_COUNT     8
#define STEPPERS_COUNT  3
#define SUPPLY_TIMEOUT  1000

const uint8_t steps[STEPS_COUNT][4] = {{1, 0, 0, 0},
                                       {1, 1, 0, 0},
                                       {0, 1, 0, 0},
                                       {0, 1, 1, 0},
                                       {0, 0, 1, 0},
                                       {0, 0, 1, 1},
                                       {0, 0, 0, 1},
                                       {1, 0, 0, 1}};

GPIO_TypeDef* Coils_GPIO[STEPPERS_COUNT][4] =
    {{GPIOA, GPIOA, GPIOA, GPIOA},
     {GPIOC, GPIOC, GPIOC, GPIOC},
     {GPIOC, GPIOC, GPIOC, GPIOC}};

uint16_t Coils_Pins[STEPPERS_COUNT][4] =
    {{GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5},
     {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3 },
     {GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7 }};

__IO int16_t StepperPosition[3]    = {0, 0, 0};
__IO int16_t StepperTimeout[3]     = {0, 0, 0};
     int16_t StepperDestination[3] = {0, 0, 0};

void Stepper_Init(void)
{
  GPIO_InitTypeDef         GPIO_InitStructure;

  /* GPIO Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  /* Configure Stepper outputs */
  GPIO_InitStructure.GPIO_Pin =
      GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =
      GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
      GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void Stepper_Set_Step(uint8_t stepper, uint8_t step)
{
  for (uint8_t coil = 0; coil < 4; coil++) {
    if (steps[step % STEPS_COUNT][coil] == 0)
      GPIO_ResetBits(Coils_GPIO[stepper][coil], Coils_Pins[stepper][coil]);
    else
      GPIO_SetBits(Coils_GPIO[stepper][coil], Coils_Pins[stepper][coil]);
  }
}

void Stepper_Shutdown(uint8_t stepper)
{
  for (uint8_t coil = 0; coil < 4; coil++)
    GPIO_ResetBits(Coils_GPIO[stepper][coil], Coils_Pins[stepper][coil]);
}

void Stepper_Set_Destination(uint8_t stepper, int16_t position)
{
  StepperDestination[stepper] = position;
}

void Stepper_Next_Step()
{
  for (uint8_t stepper = 0; stepper < STEPPERS_COUNT; ++stepper) {

    /* No moving. Wait same time and power off all coils */
    if (StepperPosition[stepper] == StepperDestination[stepper]) {
      if (StepperTimeout[stepper] >= SUPPLY_TIMEOUT)
        Stepper_Shutdown(stepper);
      else
        StepperTimeout[stepper]++;
      continue;
    }

    /* Prevent shutdown during rotation */
    StepperTimeout[stepper] = 0;

    /* Move to one step forward or backward */
    if (StepperPosition[stepper] < StepperDestination[stepper])
      StepperPosition[stepper]++;
    else
      StepperPosition[stepper]--;

    Stepper_Set_Step(stepper, StepperPosition[stepper]);
  }
}