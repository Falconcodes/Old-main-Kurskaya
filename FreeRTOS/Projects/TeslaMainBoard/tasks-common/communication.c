#include "CommonConfig.h"

/*-----------------------------------------------------------*/

#define mainBUZZER_ON_TIME_MS                   ( 50 / portTICK_PERIOD_MS )

/* The event group used by all tasks to detect "locks" states. */
extern EventGroupHandle_t xOpenedLocks;
extern EventGroupHandle_t xProjectorEvents;
extern EventGroupHandle_t xTimeMachineEvents;

/* Current month and day */
extern uint16_t currentYear, currentMonth, currentDay;
extern uint16_t currentHour, currentMin, currentSec;

/* TimeMachine NIXIE display */
extern uint32_t previousDisplayValue;

/*-----------------------------------------------------------*/

void writeLog(const char *message, char type)
{
  xprintf("<L");

  switch (type) {
  case 'N':
  case 'F':
  case 'W':
    xprintf("%c", type);
    break;
  default:
    xprintf(" ");
  }

  xprintf("[%02d.%02d.%02d-%02d:%02d:%02d] ", currentDay, currentMonth,
          currentYear, currentHour, currentMin, currentSec);
  xprintf(message);
  xprintf("\n");
}

/*-----------------------------------------------------------*/

void prvCommunicationTask( void *pvParameters )
{
  /* USART commands:
   * >? -- get confirmation >OK
   * >T -- get date and time
   * >Rns -- set relay n (0..F) to state s (0 or 1)
   * >Svdd -- set date: v (Y,M,D,h,m,s) to value dd
   * >Ux -- translate x to USART2
   * >Mx -- set mode x (M - manual, no quest autostart; A - automatic)
   * >Lx -- log data x
   * >Ax -- allow task x start
   * >! -- reboot device
   */
  for( ;; ) {
    signed char rxChar = 0;
    STM_TESLA_LEDOff(LED_RED); /* Nothing expected */

    /* === Get first character === */
    xSerialGetChar( serCOM1, &rxChar, portMAX_DELAY );
    STM_TESLA_LEDOn(LED_RED); /* Expect more characters */

    /* Analyse preamble */
    if (rxChar != '>') continue;

    /* === Get second character === */
    xSerialGetChar( serCOM1, &rxChar, portMAX_DELAY );

    /* Analyse command */
    signed char command = rxChar;
    switch (command) {

    case 'T': {
      xprintf("<T=%02d.%02d.%02d-%02d:%02d:%02d\n", currentDay, currentMonth,
              currentYear, currentHour, currentMin, currentSec);
      break;
    }

    case 'R': {
      /* === Get relay number (0..F) === */
      uint8_t relay = 0;
      xSerialGetChar( serCOM1, &rxChar, portMAX_DELAY );
      if ('0' <= rxChar && rxChar <= '9')
        relay = rxChar - '0';
      else if ('A' <= rxChar && rxChar <= 'F')
        relay = rxChar - 'A' + 10;
      else if ('a' <= rxChar && rxChar <= 'f')
        relay = rxChar - 'a' + 10;
      else continue;

      /* === Get state number (1 - ON, 0 - OFF) === */
      xSerialGetChar( serCOM1, &rxChar, portMAX_DELAY );

      /* Apply state for relay */
      if (rxChar == '1')
        STM_TESLA_RELAYOn( (Relay_TypeDef)relay );
      else if (rxChar == '0')
        STM_TESLA_RELAYOff( (Relay_TypeDef)relay );
      else continue;

      /* Send response */
      xprintf("<OK\n");
      break;
      }

    case 'S': {
      /* === Get setup item (M=month, D=day) === */
      uint8_t setup = 0;
      xSerialGetChar( serCOM1, &rxChar, portMAX_DELAY );
      setup = rxChar;

      /* === Get first digit of value === */
      uint8_t value = 0;
      xSerialGetChar( serCOM1, &rxChar, portMAX_DELAY );
      value += (rxChar - '0') * 10;

      /* === Get second digit of value === */
      xSerialGetChar( serCOM1, &rxChar, portMAX_DELAY );
      value += (rxChar - '0');

      /* Update RTC registers */
      switch (setup) {
      case 'Y': ds1307_setYear(value); break;
      case 'M': ds1307_setMonth(value); break;
      case 'D': ds1307_setDate(value); break;
      case 'h': ds1307_setHours(value); break;
      case 'm': ds1307_setMinutes(value); break;
      case 's': ds1307_setSeconds(value); break;
      default: continue;
      }

      if (setup != 's') /* Start RTC if clock was halted */
        ds1307_setSeconds(0);

      /* Send response */
      xprintf("<OK\n");
      break;
    }

    case 'U': {
      /* === Get value to retransmit === */
      uint8_t value = 0;
      xSerialGetChar( serCOM1, &rxChar, portMAX_DELAY );
      value = rxChar;

      /* === Retransmit obtained value === */
      vPutCharToUSARTy(value);
      break;
    }

    case 'M': {
      /* === Check mode === */
      xSerialGetChar( serCOM1, &rxChar, portMAX_DELAY );
      if (rxChar == 'M')
        xEventGroupSetBits(xOpenedLocks, ebBIT_QUEST_BLOCKED);
      else // rxChar == 'A'
        xEventGroupClearBits(xOpenedLocks, ebBIT_QUEST_BLOCKED);
      break;
    }

    case 'L': {
      /* === Check required parameter === */
      xSerialGetChar( serCOM1, &rxChar, portMAX_DELAY );
      switch (rxChar) {
      case 'L': {
        EventBits_t bits = xEventGroupGetBits(xOpenedLocks);
        xprintf("<LF [BL=%d][ST=%d][CD=%d][TM=%d][FI=%d]\n",
               (bits & ebBIT_QUEST_BLOCKED) ? 1 : 0,
               (bits & ebBIT_QUEST_STARTED) ? 1 : 0,
               (bits & ebBIT_CABINET_DONE) ? 1 : 0,
               (bits & ebBIT_TIME_MACHINE_OPEN) ? 1 : 0,
               (bits & ebBIT_FINAL_LOCK_OPEN) ? 1 : 0);
        break;
      } // case >LL
      case 'G': {
        int val1 = STM_TESLA_ANALOGGetValue(ANALOG_IN_HEAD1);
        int val2 = STM_TESLA_ANALOGGetValue(ANALOG_IN_HEAD2);
        int val3 = STM_TESLA_ANALOGGetValue(ANALOG_IN_HEAD3);
        int val4 = STM_TESLA_ANALOGGetValue(ANALOG_IN_HEAD4);
        xprintf("<LF [%4d, %4d, %4d, %4d]\n", val1, val2, val3, val4);
        break;
      } // case >LG
      case 'B': {
        EventBits_t bitsP = xEventGroupGetBits(xProjectorEvents);
        EventBits_t bitsM = xEventGroupGetBits(xTimeMachineEvents);
        xprintf("<LF [MA=%d][I1=%d][I2=%d] [CH=%d][GR=%d][GD=%d][PO=%d]\n",
               (bitsM & ebBIT_MARKS) ? 1 : 0,
               (bitsM & ebBIT_IAKOV1) ? 1 : 0,
               (bitsM & ebBIT_IAKOV2) ? 1 : 0,
               (bitsP & ebBIT_CHARGE) ? 1 : 0,
               (bitsP & ebBIT_GRAPHITE) ? 1 : 0,
               (bitsP & ebBIT_GRAPHITE_DONE) ? 1 : 0,
               (bitsP & ebBIT_PROJECTOR_OPENED) ? 1 : 0);
        break;
      } // case >LB
      case 'M': {
        xprintf("<LF [%d]\n", previousDisplayValue);
        break;
      } // case >LM
      } // switch >Lx
      break;
    }

    case 'A': {
      /* === Check required parameter === */
      xSerialGetChar( serCOM1, &rxChar, portMAX_DELAY );
      switch (rxChar) {
      case 'S': xEventGroupSetBits(xOpenedLocks, ebBIT_QUEST_STARTED);     break;
      case 'C': xEventGroupSetBits(xOpenedLocks, ebBIT_CABINET_DONE);      break;
      case 'T': xEventGroupSetBits(xOpenedLocks, ebBIT_TIME_MACHINE_OPEN); break;
      case 'F': xEventGroupSetBits(xOpenedLocks, ebBIT_FINAL_LOCK_OPEN);   break;

      case 'R': xEventGroupSetBits(xProjectorEvents, ebBIT_CHARGE);           break;
      case 'G': xEventGroupSetBits(xProjectorEvents, ebBIT_GRAPHITE);         break;
      case 'D': xEventGroupSetBits(xProjectorEvents, ebBIT_GRAPHITE_DONE);    break;
      case 'O': xEventGroupSetBits(xProjectorEvents, ebBIT_PROJECTOR_OPENED); break;
      }
      break;
    }

    case '!': {
      STM_TESLA_BUZZEROn();
      vTaskDelay( mainBUZZER_ON_TIME_MS );
      NVIC_SystemReset();
      while(1); /* Never reaches */
    }

    case '?': xprintf("<OK\n"); break;

    default: continue;
    }
  }
}
/*-----------------------------------------------------------*/
