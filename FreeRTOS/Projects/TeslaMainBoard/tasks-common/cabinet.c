#include "CommonConfig.h"

/**
 * When phototransistor is covered (dark) it opens.
 * So "max light" value is 0 (transistor opened, connected to GND),
 * and "dark value" is ANALOG_IN_ADC_ADC_MAX_VALUE when transistor closed.
 */
#define HEAD_COVERED_ANALOG_VALUE        3820 //range 2000-4095, rising number for falling sensitivity

/*-----------------------------------------------------------*/

extern EventGroupHandle_t xOpenedLocks;
extern EventGroupHandle_t xCabinetEvents;

/*-----------------------------------------------------------*/
#ifdef ENABLE_ELECTROMECH
void prvDimmerWatcherTask( void *pvParameters )
{
  for ( ;; ) {
    /* Wait for end of wave's period (crossing zero) */
    xEventGroupWaitBits(xCabinetEvents, ebBIT_220V_CROSS_ZERO,
                        pdTRUE, pdFALSE, portMAX_DELAY);
    STM_TESLA_RELAYOff(RELAY_CAB_DIMMER);
    vTaskDelay( (DIMMER_220V_PERIOD_MS * 15) / 100 ); /* 85% brightness */
    STM_TESLA_RELAYOn(RELAY_CAB_DIMMER);
  }
}
#endif
/*-----------------------------------------------------------*/

void prvCabinetWatcherTask( void *pvParameters )
{
  /* Don't start while Time Machine is not enabled */
  xEventGroupWaitBits(xOpenedLocks,/* The event group being tested. */
                      ebBIT_QUEST_STARTED, /* The bits to wait for. */
                      pdFALSE, /* bit shouldn't be cleared. */
                      pdFALSE, /* xWaitForAllBits is not actual here. */
                      portMAX_DELAY ); /* Wait forever. */

  /* Wait for hardware to be ready */
  vTaskDelay( mainWAIT_HARDWARE_READY_MS );

  writeLog("Run Cabinet", 'N');

  for( ;; ) {
    EventBits_t uxCabinetConditions = xEventGroupGetBits(xCabinetEvents);

#ifdef ENABLE_ELECTROMECH
    /* === Check conditions for Desk Drawer === */
    if (uxCabinetConditions & ebBIT_BELL_DONE) {

      STM_TESLA_RELAYOn(RELAY_CAB_DRAWER_LOCK);
      vTaskDelay(RELAY_HOLD_TIME_MS);
      STM_TESLA_RELAYOff(RELAY_CAB_DRAWER_LOCK);
      vTaskDelay(RELAY_HOLD_TIME_MS);

      xEventGroupClearBits(xCabinetEvents, ebBIT_BELL_DONE);
      xEventGroupSetBits(xCabinetEvents, ebBIT_DRAWER_OPENED);
    }

    /* === Check conditions for Self in Cupboard === */
    if (uxCabinetConditions & ebBIT_BUTTONS_DONE) {

      /* Ensure that buttons still pressed */
      if (STM_TESLA_INPUTGetState(INPUT_7X7KNOPKI) == (uint8_t)Bit_RESET) {
        STM_TESLA_RELAYOn(RELAY_CAB_SHELF_LOCK);
        vTaskDelay(RELAY_HOLD_TIME_MS);
        STM_TESLA_RELAYOff(RELAY_CAB_SHELF_LOCK);

        xEventGroupSetBits(xCabinetEvents, ebBIT_SELF_OPENED);
      }
      xEventGroupClearBits(xCabinetEvents, ebBIT_BUTTONS_DONE);
    }
#endif

    /* === Check that the head is covered (4 transistors are "opened") === */
    if (STM_TESLA_ANALOGGetValue(ANALOG_IN_HEAD1) > HEAD_COVERED_ANALOG_VALUE &&
        STM_TESLA_ANALOGGetValue(ANALOG_IN_HEAD2) > HEAD_COVERED_ANALOG_VALUE &&
        STM_TESLA_ANALOGGetValue(ANALOG_IN_HEAD3) > HEAD_COVERED_ANALOG_VALUE &&
        STM_TESLA_ANALOGGetValue(ANALOG_IN_HEAD4) > HEAD_COVERED_ANALOG_VALUE)
      xEventGroupSetBits(xCabinetEvents, ebBIT_HEAD_COVERED);
    else
      xEventGroupClearBits(xCabinetEvents, ebBIT_HEAD_COVERED);

    /* === Check that the key was rotated === */
    if (STM_TESLA_INPUTGetState(INPUT_KEYHOLE) == (uint8_t)Bit_RESET)
      xEventGroupSetBits(xCabinetEvents, ebBIT_KEY_ROTATED);
    else
      xEventGroupClearBits(xCabinetEvents, ebBIT_KEY_ROTATED);

    /* === Check ALL conditions for exit === */
    if ((uxCabinetConditions & ebCABINET_IN_CONDITIONS) == ebCABINET_IN_CONDITIONS) {

      STM_TESLA_RELAYOn(RELAY_CAB_LIGHT_DIM);
      vTaskDelay( mainWAIT_HARDWARE_READY_MS );
      STM_TESLA_RELAYOff(RELAY_CAB_LOCK_OUT);
      STM_TESLA_RELAYOff(RELAY_LAB_LIGHT_OFF);

      xEventGroupSetBits(xOpenedLocks, ebBIT_CABINET_DONE);

      writeLog("Cabinet done", 'N');

      /* Suspend ourselves. */
      vTaskSuspend( NULL );
    }

    /* Wait before next checking cycle. */
    vTaskDelay( mainMIN_CHECKING_DELAY_MS );
  }
}
/*-----------------------------------------------------------*/

#ifdef ENABLE_ELECTROMECH
/* +++ EXTI handler (dimmer, bell and 7x7 buttons panel) */
void EXTI4_15_IRQHandler(void)
{
  /* ATTENTION! Here should be used function
        xEventGroupSetBitsFromISR()
     instead of
        xEventGroupSetBits()
     but MCU have no enough memory to enable this function. */

  if (EXTI_GetITStatus(STM_TESLA_EXTILine(INPUT_BELL)) == SET) {
    if (STM_TESLA_INPUTGetState(INPUT_BELL) == (uint8_t)Bit_RESET)
      xEventGroupSetBits(xCabinetEvents, ebBIT_BELL_DONE);
    EXTI_ClearITPendingBit(STM_TESLA_EXTILine(INPUT_BELL));
  }

  if (EXTI_GetITStatus(STM_TESLA_EXTILine(INPUT_7X7KNOPKI)) == SET) {
    xEventGroupSetBits(xCabinetEvents, ebBIT_BUTTONS_DONE);
    EXTI_ClearITPendingBit(STM_TESLA_EXTILine(INPUT_7X7KNOPKI));
  }

  if (EXTI_GetITStatus(STM_TESLA_EXTILine(INPUT_220V_SENSE)) == SET) {
    xEventGroupSetBits(xCabinetEvents, ebBIT_220V_CROSS_ZERO);
    EXTI_ClearITPendingBit(STM_TESLA_EXTILine(INPUT_220V_SENSE));
  }
}
#endif