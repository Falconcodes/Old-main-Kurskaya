#include "CommonConfig.h"

/*-----------------------------------------------------------*/

extern EventGroupHandle_t xOpenedLocks;

extern uint16_t currentMonth, currentDay;

uint32_t previousDisplayValue = 0, correct_date_wait = 0;
uint8_t busy_flag = 0;
uint8_t nixiePosMap[8] = { 0, 1, 3, 2, 5, 4, 6, 7 };

/* UART-NIXIE match:          0  1  2  3  4  5  6  7  8  9 */
uint8_t nixieDigitMap[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
/*-----------------------------------------------------------*/

uint8_t checkCorrectDate(uint16_t month, uint16_t day)
{
  if (month == currentMonth && day == currentDay)
    return 1;
  else
    return 0;
}
/*-----------------------------------------------------------*/

void Set_NIXIE_Digit(uint8_t digit, uint8_t pos)
{
  /* Check input data */
  if (digit > 10 || pos > 7) /* "10" turns digit off */
    return;

  /* Send value to NIXIE device over USART */
  vPutCharToUSARTy(((nixiePosMap[pos] & 0x0F) << 4) |
                    (nixieDigitMap[digit] & 0x0F));
}
/*-----------------------------------------------------------*/

void Set_NIXIE_Value(uint32_t value)
{
  /* Write value */
  /* For 8-digit display range is 0000 0000 - 9999 9999 */

  uint32_t divider = 10000000;
  uint32_t currentValue;

  /* Check value for overflow */
  if (value / divider > 9)
    currentValue = 99999999;
  else
    currentValue = value;

  /* Store values */
  uint8_t position = 7; /* Positions: 0 1 2 3 4 5 6 7 */
  while (divider >= 1) {
    uint32_t currentDigit = currentValue / divider;
    currentValue -= currentDigit * divider;
    divider /= 10;

    Set_NIXIE_Digit(currentDigit, position);
    --position;
  }
}
/*-----------------------------------------------------------*/

uint16_t getMonthFromRaw(uint16_t raw_month)
{
  uint16_t min = 280;
  uint16_t max = 4070;

  if (raw_month < min)
    return 0;

  uint16_t val = (((raw_month-min) * 11) / (max-min)) + 1; /* range: 01..12 */
  return (val < 12) ? val : 12;
}

uint16_t getDayFromRaw(uint16_t raw_day)
{
  uint16_t min = 425;
  uint16_t max = 4080;

  if (raw_day < min)
    return 0;

  uint16_t val = (((raw_day-min) * 30) / (max-min)) + 1; /* range: 01..31 */
  return (val < 31) ? val : 31;
}
/*-----------------------------------------------------------*/

uint8_t rightDateSet = 0;

void prvTimeMachineWatcherTask( void *pvParameters )
{
  /* Don't start while Time Machine is not enabled */
  xEventGroupWaitBits(xOpenedLocks,/* The event group being tested. */
                      ebBIT_TIME_MACHINE_OPEN, /* The bits to wait for. */
                      pdFALSE, /* bit shouldn't be cleared. */
                      pdFALSE, /* xWaitForAllBits is not actual here. */
                      portMAX_DELAY ); /* Wait forever. */

  /* Wait for hardware to be ready */
  vTaskDelay( mainWAIT_IAKOV_OFF_MS );
  STM_TESLA_RELAYOn(RELAY_LAB_LIGHT_OFF); //ДН свет гаснет после задымления машины (реле типа НЗ)
  STM_TESLA_RELAYOff(RELAY_LAB_IAKOV1);   //в этот молмент выключаем первую лестницу
  STM_TESLA_RELAYOff(RELAY_LAB_IAKOV2);   //и вторую тоже
  
  for( ;; ) {
    /* Read values from ADC and display on NIXIE */
    uint16_t raw_month = STM_TESLA_ANALOGGetValue(ANALOG_IN_RMON5);
    uint16_t raw_day = STM_TESLA_ANALOGGetValue(ANALOG_IN_RDAY6);
    uint16_t monthValue, dayValue;
    uint32_t displayValue;

    /* Calculate month and day values */
    /* actual_value = ( ( values[CHANNEL] + LIMITER*MAX_VALUE ) / ADC_MAX_VALUE
                                                             + SHIFT_VALUE ) */
    monthValue = getMonthFromRaw(raw_month);
    dayValue   = getDayFromRaw(raw_day);
    displayValue = (2015) +
                   (uint32_t)monthValue*10000 +
                   (uint32_t)dayValue*1000000;

    /* Send data to NIXIE device */
    if (displayValue != previousDisplayValue && !rightDateSet) {
      Set_NIXIE_Value(displayValue);
      previousDisplayValue = displayValue;
    }

    //check for correct date, considering analog drift
    if (checkCorrectDate(monthValue,   dayValue  ) |
	checkCorrectDate(monthValue+1, dayValue  ) |
	checkCorrectDate(monthValue+1, dayValue+1) |
	checkCorrectDate(monthValue,   dayValue+1) |
	checkCorrectDate(monthValue-1, dayValue  ) |
	checkCorrectDate(monthValue-1, dayValue-1) |
        checkCorrectDate(monthValue,   dayValue-1) |
        checkCorrectDate(monthValue-1, dayValue+1) |
        checkCorrectDate(monthValue+1, dayValue-1)
          ) correct_date_wait++; //increment if date is true +/- 1 day or month in this cycle, +/- is made for solve analog drift
    else correct_date_wait=0; //or else count-variable is set to 0.
    
    if (correct_date_wait == CORRECT_DATE_WAIT_COUNT) {
      rightDateSet = 1;   //if waiting with correct date was success, apply it
      Set_NIXIE_Value((2015) + (uint32_t)currentMonth*10000 + (uint32_t)currentDay*1000000); //set CORRECT date on display, even if last values was +1 or -1 from correct
      STM_TESLA_RELAYOn(RELAY_4); //turn-on buttons backlight
    }
    
    /* Check for correct date and push on green buttons*/
    if (rightDateSet && STM_TESLA_INPUTGetState(INPUT_5) == (uint8_t)Bit_RESET) {
     
      //green button backlight off 
      STM_TESLA_RELAYOff(RELAY_4);
      
      /* Open FINAL lock */
      STM_TESLA_RELAYOn(RELAY_LAB_TM_FINAL);
            
      vTaskDelay( 1000 );
      STM_TESLA_RELAYOff(RELAY_LAB_TM_FINAL);

      writeLog("TM done", 'N');

      /* Set bit "lock to Time Machine is opened" */
      xEventGroupSetBits(xOpenedLocks, ebBIT_FINAL_LOCK_OPEN);

      /* [TODO] Insert here "Final animation" */
      STM_TESLA_BUZZEROn();
      vTaskDelay( 3000 );
      STM_TESLA_BUZZEROff();
      
        /* Do not suspend, because it is possible to close
         doors to time machine AFTER press 4 green buttons. */
    }

    /* Wait before next checking cycle. */
    vTaskDelay( mainMIN_CHECKING_DELAY_MS );
  }
}
/*-----------------------------------------------------------*/