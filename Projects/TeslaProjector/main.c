/**
  ******************************************************************************
  * @file    TeslaProjector/main.c
  * @author  Yuriy (yurboc@yandex.ru)
  * @version V1.0
  * @date    24-December-2014
  * @brief   Device which used to control Raspberry Pi, connected
             to projector. Send 'E' to enable video, 'D' to disable.
  ******************************************************************************
  */

#include "main.h"
#include "usart.h"
#include "encoder.h"

#define SELF_ROTATING_TIME 3

__IO uint16_t last_encoder_value = 0;
__IO uint16_t last_rotating_state = 0;
__IO uint16_t is_rotating = 0;

int main(void)
{
  /* Configure SysTick */
  if (SysTick_Config(SystemCoreClock / 1000))
    while (1);

  /* Configure USART on PA2(TX), PA3(RX) */
  USARTx_Init();

  /* Configure encoder on  */
  Encoder_Init();

  /* Read encoder in loop */
  while (1)
  {
    Delay(500);

    uint16_t new_encoder_value = Encoder_Get_Value();

    if (new_encoder_value > last_encoder_value)
      is_rotating = SELF_ROTATING_TIME;
    else if (is_rotating > 0)
      --is_rotating;

    last_encoder_value = new_encoder_value;

    if (is_rotating == 0 && last_rotating_state != 0) { /* stop rotation */
      USARTx_Write_Byte('p');
      last_rotating_state = 0;
      continue;
    }

    if (is_rotating > 0 && last_rotating_state == 0) { /* begin rotation */
      USARTx_Write_Byte('p');
      last_rotating_state = 1;
      continue;
    }
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

