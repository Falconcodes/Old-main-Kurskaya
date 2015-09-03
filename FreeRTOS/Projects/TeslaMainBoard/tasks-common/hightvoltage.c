#include "CommonConfig.h"

/*-----------------------------------------------------------*/

#define HV_DELAYED_START_S       3      // 3 seconds
#define HV_REENABLE_TIMOUT_S    10      // 10 seconds
#define HV_DISABLE_TIMOUT_S     20      // 20 seconds

extern EventGroupHandle_t xOpenedLocks;
extern EventGroupHandle_t xTimeMachineEvents;
extern struct TimerDelayer timerDelayer;

uint16_t iakov_1_start_wait = 0, iakov_2_start_wait = 0;

/*-----------------------------------------------------------*/

void prvTimeMachineEnablerTask( void *pvParameters )
{
  /* Don't start while laboratory is not opened */
  xEventGroupWaitBits(xOpenedLocks, /* The event group being tested. */
                      ebBIT_CABINET_DONE, /* The bits to wait for. */
                      pdFALSE, /* ebBIT_CABINET_DONE shouldn't be cleared. */
                      pdFALSE, /* xWaitForAllBits is not actual here. */
                      portMAX_DELAY ); /* Wait forever. */

  /* Wait for hardware to be ready */
  vTaskDelay( mainWAIT_HARDWARE_READY_MS );

  for( ;; ) {



    /* === Check conditions for Marks Generator === */
    EventBits_t uxTimeMachineState = xEventGroupGetBits(xTimeMachineEvents);
    if (STM_TESLA_INPUTGetState(INPUT_RUBILNIK) == (uint8_t)Bit_RESET) {
      xEventGroupSetBits(xTimeMachineEvents, ebBIT_MARKS);
      STM_TESLA_RELAYOn(RELAY_LAB_MARKS);
    }
    else {
      xEventGroupClearBits(xTimeMachineEvents, ebBIT_MARKS);
      STM_TESLA_RELAYOff(RELAY_LAB_MARKS);
    }



    /* === Check conditions for Iakov_1 Device === */
    if (STM_TESLA_INPUTGetState(INPUT_TUMBLERY) == (uint8_t)Bit_SET) {
      iakov_1_start_wait++; 
      
      if (iakov_1_start_wait == START_IAKOV_WAIT_COUNT) {
        xEventGroupSetBits(xTimeMachineEvents, ebBIT_IAKOV1);
        STM_TESLA_RELAYOn(RELAY_LAB_IAKOV1);
        iakov_1_start_wait = 0;
      }
    }  
    else {
    iakov_1_start_wait = 0;
    //  xEventGroupClearBits(xTimeMachineEvents, ebBIT_IAKOV1);
    //  STM_TESLA_RELAYOff(RELAY_LAB_IAKOV1);
    }



    /* === Check conditions for Iakov_2 Device === */
    if (STM_TESLA_INPUTGetState(INPUT_RADIOLA) == (uint8_t)Bit_SET) {
      iakov_2_start_wait++;
      
      if (iakov_2_start_wait == START_IAKOV_WAIT_COUNT) {
        xEventGroupSetBits(xTimeMachineEvents, ebBIT_IAKOV2);
        STM_TESLA_RELAYOn(RELAY_LAB_IAKOV2);
        iakov_2_start_wait = 0;
      
      }
    }  
    else {
    iakov_2_start_wait = 0;
    //  xEventGroupClearBits(xTimeMachineEvents, ebBIT_IAKOV2);
    //  STM_TESLA_RELAYOff(RELAY_LAB_IAKOV2);
    }



    /* +++ Update "TimeMachine lock" state +++ */
    EventBits_t uxMetConditions = xEventGroupGetBits(xTimeMachineEvents);
    if ((uxMetConditions & ebMACHINE_IN_CONDITIONS) == ebMACHINE_IN_CONDITIONS){

      /* Open lock to Time Machine right now */
      STM_TESLA_RELAYOn(RELAY_LAB_TM_START);

      /* Set bit "lock to Time Machine is opened" */
      xEventGroupSetBits(xOpenedLocks, ebBIT_TIME_MACHINE_OPEN);

      vTaskDelay( 1000 /* 1000 ms */ );
      STM_TESLA_RELAYOff(RELAY_LAB_TM_START);
      
      //две строки, которые ниже, перенес в файл timemachine, после выключения света в лаборатории (задержка mainWAIT_IAKOV_OFF_MS после начала задымления)
      //STM_TESLA_RELAYOff(RELAY_LAB_IAKOV1);
      //STM_TESLA_RELAYOff(RELAY_LAB_IAKOV2);

      STM_TESLA_RELAYOn(RELAY_LAB_LIGHT_OFF);

      writeLog("Laboratory done", 'N');

      vTaskSuspend( NULL );
    }
 
    /* Wait before next checking cycle. */
    vTaskDelay( mainMIN_CHECKING_DELAY_MS );
  }
}
/*-----------------------------------------------------------*/
