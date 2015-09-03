#include "nixie_driver.h"

static uint8_t currentBulb = 0;
volatile uint32_t displayData[NIXIE_BULBS_COUNT];

void NIXIE_Init(void)
{
  /* Display zero */
  for (int i = 0; i < NIXIE_BULBS_COUNT; ++i)
    displayData[i] = 0;

  NIXIE_BulbsOff(DigitAll);
}

void NIXIE_BulbsOn(uint32_t pos)
{
  /* Always enable only ONE bulb (overcurrent may damage HV supply) */
  switch (pos) {
  case Digit1A: PORTB |= (1 << 0); break;
  case Digit1B: PORTB |= (1 << 1); break;
  case Digit2A: PORTB |= (1 << 2); break;
  case Digit2B: PORTB |= (1 << 3); break;
  case Digit3A: PORTB |= (1 << 4); break;
  case Digit3B: PORTB |= (1 << 5); break;
  case Digit3C: PORTB |= (1 << 6); break;
  case Digit3D: PORTB |= (1 << 7); break;
  }
}

void NIXIE_BulbsOff(uint32_t pos)
{
  if ((pos & Digit1A) != 0) PORTB &= ~(1 << 0);
  if ((pos & Digit1B) != 0) PORTB &= ~(1 << 1);
  if ((pos & Digit2A) != 0) PORTB &= ~(1 << 2);
  if ((pos & Digit2B) != 0) PORTB &= ~(1 << 3);
  if ((pos & Digit3A) != 0) PORTB &= ~(1 << 4);
  if ((pos & Digit3B) != 0) PORTB &= ~(1 << 5);
  if ((pos & Digit3C) != 0) PORTB &= ~(1 << 6);
  if ((pos & Digit3D) != 0) PORTB &= ~(1 << 7);
}

void NIXIE_SetDigit(uint32_t id)
{
  /* Calculate address */
  uint8_t address;
  if (id < NIXIE_DIGITS_COUNT)
    address = CathodeMap[id];
  else
    address = NIXIE_INVALID_ADDRESS;

  /* Write address to the port */
  for (int i = 0; i < NIXIE_ADDRESS_WIDTH; ++i) {
    if ((address & (1<<i)) != 0)
      PORTC |= (1 << i);
    else
      PORTC &= ~(1 << i);
  }
}

void NIXIE_NextTick(void)
{
  /* Light next bulb */
  ++currentBulb;
  currentBulb %= NIXIE_BULBS_COUNT;

  /* Disable all anodes */
  NIXIE_BulbsOff(DigitAll);

  /* Set new address on port */
  NIXIE_SetDigit(displayData[currentBulb]);

  /* Enable anode */
  NIXIE_BulbsOn(1 << currentBulb);
}

void NIXIE_WriteDigit(uint8_t digit, uint8_t pos)
{
  /* Limit position to bulbs count */
  if (pos >= NIXIE_BULBS_COUNT)
    return;

  /* Store data to display buffer */
  displayData[pos] = digit;
}

void NIXIE_WriteValue(uint32_t value)
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

    NIXIE_WriteDigit(currentDigit, position);
    --position;
  }
}

void NIXIE_Off()
{
  /* Display all invalid values (NIXIE_DIGITS_COUNT = max_valid_value + 1) */
  for (int i = 0; i < NIXIE_BULBS_COUNT; ++i)
    displayData[i] = NIXIE_DIGITS_COUNT;
}
