#include "CommonConfig.h"

/* Priorities at which the tasks are created. */
#define mainHEARTBEAT_TASK_PRIORITY             ( tskIDLE_PRIORITY )
#define mainWATCHER_TASK_PRIORITY               ( tskIDLE_PRIORITY + 1 )
#define mainREALTIME_TASK_PRIORITY              ( tskIDLE_PRIORITY + 2 )

/* Tasks which use serial port requires large stack size, otherwise overflow */
#define mainUSART_RELATED_TASK_STACK_SIZE       ( ( unsigned short ) 128 )

/* The rate at which HeartBeat LED is toggled (1000ms). */
#define mainHEARTBEAT_ON_TIME_MS                ( 10 / portTICK_PERIOD_MS )
#define mainHEARTBEAT_OFF_TIME_MS               ( 990 / portTICK_PERIOD_MS )

/* Time before quest autostart (3000ms). */
#define mainQUEST_AUTOSTART_TIME_MS             ( 3000 / portTICK_PERIOD_MS )

/*-----------------------------------------------------------*/

/* Called by main() to create the Main Monitor application. */
void main_monitor( void );

/* Toggle green LED to indicate board state */
void prvHeartbeatTask( void *pvParameters );

/* Reads commands from USART and sends answers */
extern void prvCommunicationTask( void *pvParameters );

#ifdef ENABLE_ELECTROMECH
/* Controls Dimmer on 220V AC line */
extern void prvDimmerWatcherTask( void *pvParameters );
#endif

/* Detects conditions in the room "Cabinet" */
extern void prvCabinetWatcherTask( void *pvParameters );

/* Detects conditions of "Enable Time Machine" */
extern void prvTimeMachineEnablerTask( void *pvParameters );

/* Detects conditions of "Right Date on Time Machine" */
extern void prvTimeMachineWatcherTask( void *pvParameters );

/* Detects conditions of "Enable Projector" */
extern void prvProjectorEnablerTask( void *pvParameters );

/* Detects conditions of "Start Video on Projector" */
extern void prvProjectorWatcherTask( void *pvParameters );

/* Animate digits on the NIXIE indicator */
extern void prvTimeAnimationTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* The event group used by all tasks to detect "locks" states. */
EventGroupHandle_t xOpenedLocks = NULL;

/* The event group used by "Cabinet" task. */
EventGroupHandle_t xCabinetEvents = NULL;

/* The event group used by "Laboratory" task to detect "TimeMachine" state. */
EventGroupHandle_t xTimeMachineEvents = NULL;

/* The event group used by "Laboratory" task to detect "Projector" state. */
EventGroupHandle_t xProjectorEvents = NULL;

/* Current month and day */
volatile uint16_t currentYear = 0, currentMonth = 0, currentDay = 0;
         uint16_t currentHour = 0, currentMin = 0, currentSec = 0;

/*===========================================================*/

void main_monitor( void )
{
  /* Create the event groups. */
  xOpenedLocks = xEventGroupCreate();
  xCabinetEvents = xEventGroupCreate();
  xProjectorEvents = xEventGroupCreate();
  xTimeMachineEvents = xEventGroupCreate();

  /* Create tasks. */
  xTaskCreate( prvHeartbeatTask, /* The function that implements the task. */
              "HeartBeat",       /* The text name assigned to the task. */
              mainUSART_RELATED_TASK_STACK_SIZE, /* The size of the stack to allocate to the task. */
              NULL,              /* The parameter passed to the task. */
              mainHEARTBEAT_TASK_PRIORITY, /* The priority assigned to the task. */
              NULL );            /* The task handle is not required, so NULL is passed. */

  xTaskCreate( prvCommunicationTask,
              "Serial_Port_Communication",
              mainUSART_RELATED_TASK_STACK_SIZE,
              NULL,
              mainWATCHER_TASK_PRIORITY,
              NULL );

#ifdef ENABLE_ELECTROMECH
  xTaskCreate( prvDimmerWatcherTask,
              "Dimmer_Controller",
              configMINIMAL_STACK_SIZE,
              NULL,
              mainREALTIME_TASK_PRIORITY,
              NULL );
#endif

  xTaskCreate( prvCabinetWatcherTask,
              "Cabinet_Watcher",
              mainUSART_RELATED_TASK_STACK_SIZE,
              NULL,
              mainWATCHER_TASK_PRIORITY,
              NULL );

  xTaskCreate( prvTimeMachineEnablerTask,
              "TimeMachine_Enabler",
              mainUSART_RELATED_TASK_STACK_SIZE,
              NULL,
              mainWATCHER_TASK_PRIORITY,
              NULL );

  xTaskCreate( prvTimeMachineWatcherTask,
              "TimeMachine_Watcher",
              mainUSART_RELATED_TASK_STACK_SIZE,
              NULL,
              mainWATCHER_TASK_PRIORITY,
              NULL );

  xTaskCreate( prvProjectorEnablerTask,
              "Projector_Enabler",
              configMINIMAL_STACK_SIZE,
              NULL,
              mainWATCHER_TASK_PRIORITY,
              NULL );

  xTaskCreate( prvProjectorWatcherTask,
              "Projector_Watcher",
              configMINIMAL_STACK_SIZE,
              NULL,
              mainWATCHER_TASK_PRIORITY,
              NULL );

  xTaskCreate( prvTimeAnimationTask,
              "Time_Animator",
              configMINIMAL_STACK_SIZE,
              NULL,
              mainWATCHER_TASK_PRIORITY,
              NULL );

  /* Start the tasks and timer running. */
  vTaskStartScheduler();

  /* If all is well, the following line will never be reached. */
  for( ;; );
}
/*-----------------------------------------------------------*/

static void prepareQuest()
{
  /* Prepare Cabinet */
  STM_TESLA_RELAYOn(RELAY_CAB_LOCK_OUT);

  /* Prepare laboratory */
  STM_TESLA_RELAYOn(RELAY_LAB_LIGHT_OFF);
}
/*-----------------------------------------------------------*/

static void readCurrentTime( void )
{
  currentYear  = ds1307_getYear();
  currentMonth = ds1307_getMonth();
  currentDay   = ds1307_getDate();
  currentHour  = ds1307_getHours();
  currentMin   = ds1307_getMinutes();
  currentSec   = ds1307_getSeconds();
}
/*-----------------------------------------------------------*/

static void prvHeartbeatTask( void *pvParameters )
{
  /* Read current time and date */
  readCurrentTime();

  /* Check if autostart allowed */
  xprintf("<M?\n");

  /* Wait some time before autostart (autostart can be blocked remotely) */
  EventBits_t bits =
    xEventGroupWaitBits(xOpenedLocks,/* The event group being tested. */
                        ebBIT_QUEST_BLOCKED, /* The bits to wait for. */
                        pdTRUE, /* bit should be cleared. */
                        pdFALSE, /* xWaitForAllBits is not actual here. */
                        mainQUEST_AUTOSTART_TIME_MS ); /* Waiting time. */

  /* Allow autostart if quest wasn't blocked during timeout */
  if ((bits & ebBIT_QUEST_BLOCKED) == 0) {
    writeLog("NORMAL mode", 'N');
    xEventGroupSetBits(xOpenedLocks, ebBIT_QUEST_STARTED);

    /* Set pre-conditions */
    prepareQuest();
  }
  else
    writeLog("MANUAL mode", 'W');

  for( ;; ) {
    /* Turn on HeartBeat LED */
    STM_TESLA_LEDOn(LED_GREEN);
    vTaskDelay( mainHEARTBEAT_ON_TIME_MS );

    /* Turn off HeartBeat LED */
    STM_TESLA_LEDOff(LED_GREEN);
    vTaskDelay( mainHEARTBEAT_OFF_TIME_MS );

    /* Read current time and date */
    readCurrentTime();
  }
}
/*-----------------------------------------------------------*/