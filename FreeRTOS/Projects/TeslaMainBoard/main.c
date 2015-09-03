/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/*-----------------------------------------------------------*/

/* Perform application specific hardware configuration. */
static void prvSetupHardware( void );
extern void vBoardHardwareInitialize( void );

/* Monitor routine. */
extern void main_monitor( void );

/*-----------------------------------------------------------*/

int main( void )
{
  /* Prepare the hardware for run application. */
  prvSetupHardware();

  /* Start Main Monitor routine. */
  main_monitor();

  return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
  vBoardHardwareInitialize();
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
  taskDISABLE_INTERRUPTS();
  for( ;; );
}

void vApplicationIdleHook( void )
{
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
  ( void ) pcTaskName;
  ( void ) pxTask;
  taskDISABLE_INTERRUPTS();
  for( ;; );
}

void vApplicationTickHook( void )
{
}
/*-----------------------------------------------------------*/