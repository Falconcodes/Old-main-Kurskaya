#include "CommonConfig.h"

/*-----------------------------------------------------------*/

extern EventGroupHandle_t xOpenedLocks;

extern void Set_NIXIE_Digit(uint8_t digit, uint8_t pos);

/*-----------------------------------------------------------*/

void prvTimeAnimationTask( void *pvParameters )
{
  /* Don't start while projector cover is not opened */
  xEventGroupWaitBits(xOpenedLocks, /* The event group being tested. */
                      ebBIT_FINAL_LOCK_OPEN, /* The bits to wait for. */
                      pdFALSE, /* bit shouldn't be cleared. */
                      pdFALSE, /* xWaitForAllBits is not actual here. */
                      portMAX_DELAY ); /* Wait forever. */

  /* Set initial value */
  Set_NIXIE_Digit(0, 7);
  Set_NIXIE_Digit(9, 6);
  Set_NIXIE_Digit(0, 5);
  Set_NIXIE_Digit(6, 4);
  Set_NIXIE_Digit(1, 3);
  Set_NIXIE_Digit(9, 2);
  Set_NIXIE_Digit(3, 1);
  Set_NIXIE_Digit(2, 0);

  /* Wait for hardware to be ready */
  vTaskDelay( 5000 );

  int sh = 0;

  for( ;; ) {

    /* === Digits animation === */
    int val = 1;
    sh += 1;
    sh %= 8;

    for (int i=0; i<8; ++i) {
      val = i + sh;
      val %= 8;
      Set_NIXIE_Digit(val + 1, 7-i);
    }

    /* Wait before next cycle. */
    vTaskDelay( 100 );
  }
}
/*-----------------------------------------------------------*/