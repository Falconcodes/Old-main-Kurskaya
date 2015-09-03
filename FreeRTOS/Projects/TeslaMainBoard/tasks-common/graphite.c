#include "CommonConfig.h"

//коэффициент замедления, чем он больше, тем медленнее стрелка динамо-машины набирает показания
#define SLOW_RATE 2 //decrease rate for ampermeter on dinamo 

#define GENERATOR_MAXIMUM_CHARGE        ( 60*SLOW_RATE )

/*-----------------------------------------------------------*/

extern EventGroupHandle_t xOpenedLocks;
extern EventGroupHandle_t xProjectorEvents;

/*-----------------------------------------------------------*/

void prvProjectorEnablerTask( void *pvParameters )
{
  /* Don't start while laboratory is not opened */
  xEventGroupWaitBits(xOpenedLocks,/* The event group being tested. */
                      ebBIT_CABINET_DONE, /* The bits to wait for. */
                      pdFALSE, /* ebBIT_CABINET_DONE shouldn't be cleared. */
                      pdFALSE, /* xWaitForAllBits is not actual here. */
                      portMAX_DELAY ); /* Wait forever. */

  /* Wait for hardware to be ready */
  vTaskDelay( mainWAIT_HARDWARE_READY_MS );

  int16_t prevEncoderValue = 0;
  uint16_t currChargeValue = 0;

  for( ;; ) {

    /* === Charging imitation === */
    if (xEventGroupGetBits(xProjectorEvents) == 0) {

      /* Read Encoder */
      int16_t currEncoderValue = STM_TESLA_ENCODERGetValue(ENCODER_FROM_GENERATOR);
      if (currEncoderValue != prevEncoderValue) {

        /* Update "Ampermeter" (PWM) value */
        currChargeValue += 1;
        STM_TESLA_PWMSetDutyCycle(PWM_OUT_TO_GENERATOR, currChargeValue/SLOW_RATE);

      }
      prevEncoderValue = currEncoderValue;

      /* Update EventGroup if charged */
      if (currChargeValue >= GENERATOR_MAXIMUM_CHARGE)
        xEventGroupSetBits(xProjectorEvents, ebBIT_CHARGE);
    }

    /* === Graphite enabling === */
    if (xEventGroupGetBits(xProjectorEvents) == ebBIT_CHARGE) {

      /* Switch "Graphite" on */
      STM_TESLA_RELAYOn(RELAY_LAB_GRAPH_START);
      xEventGroupSetBits(xProjectorEvents, ebBIT_GRAPHITE);
    }

    /* === Graphite finishing === */
    if (xEventGroupGetBits(xProjectorEvents) == (ebBIT_CHARGE |
                                                           ebBIT_GRAPHITE)) {
      /* Check if "Graphite" is finished */
      if (STM_TESLA_INPUTGetState(INPUT_GRAPHITE) == (uint8_t)Bit_RESET)
        xEventGroupSetBits(xProjectorEvents, ebBIT_GRAPHITE_DONE);
    }

    /* === Open projector cover === */
    if (xEventGroupGetBits(xProjectorEvents)
        == (ebBIT_CHARGE | ebBIT_GRAPHITE | ebBIT_GRAPHITE_DONE)) {

      /* Open cover of the projector */
      STM_TESLA_RELAYOn(RELAY_LAB_PROJ_LOCK);
      vTaskDelay( RELAY_HOLD_TIME_MS );
      STM_TESLA_RELAYOff(RELAY_LAB_PROJ_LOCK);

      /* Turn on big green lamp on the projector */
      STM_TESLA_RELAYOn(RELAY_LAB_PROJ_GREEN);

      xEventGroupSetBits(xProjectorEvents, ebBIT_PROJECTOR_OPENED);
    }

    /* +++ Suspend this task if everything done. +++ */
    if (xEventGroupGetBits(xProjectorEvents) == ebBIT_PROJECTOR_OPENED)
      vTaskSuspend( NULL );

    /* Wait before next checking cycle. */
    vTaskDelay( mainMIN_CHECKING_DELAY_MS );
  }
}
/*-----------------------------------------------------------*/