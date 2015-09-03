#include "CommonConfig.h"

#define PROJECTOR_ANALYSE_HANDLE_MS     ( 500 / portTICK_PERIOD_MS )

/*-----------------------------------------------------------*/

extern EventGroupHandle_t xOpenedLocks;
extern EventGroupHandle_t xProjectorEvents;

/*-----------------------------------------------------------*/

void prvProjectorWatcherTask( void *pvParameters )
{
  /* Don't start while projector cover is not opened */
  xEventGroupWaitBits(xProjectorEvents, /* The event group being tested. */
                      ebBIT_PROJECTOR_OPENED, /* The bits to wait for. */
                      pdFALSE, /* bit shouldn't be cleared. */
                      pdFALSE, /* xWaitForAllBits is not actual here. */
                      portMAX_DELAY ); /* Wait forever. */

  /* Wait for hardware to be ready */
  vTaskDelay( mainWAIT_HARDWARE_READY_MS );

  uint32_t timeLimit = 0;
  int16_t prevEncoderValue = 0;
  int16_t projectorState = 0;

  for( ;; ) {

    /* === Rotate handle imitation === */

    /* Read Encoder and decide what to do: Play or Pause */
    int16_t currEncoderValue = STM_TESLA_ENCODERGetValue(ENCODER_FROM_PROJECTOR);
    if ((currEncoderValue > prevEncoderValue && projectorState == 0) ||
        (currEncoderValue <= prevEncoderValue && projectorState == 1))
      timeLimit += mainMIN_CHECKING_DELAY_MS;

    prevEncoderValue = currEncoderValue;

    /* Perform Play or Pause */
    if (timeLimit >= PROJECTOR_ANALYSE_HANDLE_MS) {
      if (projectorState == 0) {
        vPutCharToUSARTy('g'); /* Go! */
        projectorState = 1;
      }
      else {
        vPutCharToUSARTy('p'); /* Pause! */
        projectorState = 0;
      }
    }

    /* Wait before next checking cycle. */
    vTaskDelay( mainMIN_CHECKING_DELAY_MS );
  }
}
/*-----------------------------------------------------------*/