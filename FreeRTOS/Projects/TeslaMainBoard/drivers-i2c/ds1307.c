#include "ds1307.h"

#include "FreeRTOS.h"
#include "i2c_driver.h"

const char* weekday_names[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

void ds1307_writeRegister(uint8_t reg, uint8_t val)
{
  vI2CWriteRegister(reg, val);
}

uint8_t ds1307_readRegister(uint8_t reg)
{
  uint8_t result = 0;
  vI2CReadRegister(reg, &result);
  return result;
}

uint8_t bcd_to_dec(uint8_t bcd_value)
{
  uint8_t decimal;
  decimal = ((bcd_value>>4)&0x0F) * 10;
  decimal += (bcd_value&0x0F);
  return decimal;
}

uint8_t dec_to_bcd(uint8_t dec_value)
{
  uint8_t coded = 0;
  coded |= dec_value % 10;
  coded |= (dec_value / 10) << 4;
  return coded;
}



uint8_t ds1307_getSeconds()
{
  uint8_t bcd = ds1307_readRegister(DS1307_SECONDS);
  bcd &= ~(1 << 7); // ignore Clock Halt flag
  return bcd_to_dec(bcd);
}

uint8_t ds1307_getMinutes()
{
  uint8_t bcd = ds1307_readRegister(DS1307_MINUTES);
  return bcd_to_dec(bcd);
}

uint8_t ds1307_getHours()
{
  uint8_t bcd = ds1307_readRegister(DS1307_HOURS);
  bcd &= 0x3F; // Support 24-hours only (no AM/PM)
  return bcd_to_dec(bcd);
}

uint8_t ds1307_getWeekday()
{
  uint8_t bcd = ds1307_readRegister(DS1307_WEEKDAY);
  bcd &= 0x07; // Mask for weekday
  return bcd;
}

const char *ds1307_getWeekday_name()
{
  uint8_t weekday = ds1307_getWeekday();
  return weekday_names[weekday];
}

uint8_t ds1307_getDate()
{
  uint8_t bcd = ds1307_readRegister(DS1307_DATE);
  bcd &= 0x3F; // Mask for date
  return bcd_to_dec(bcd);
}

uint8_t ds1307_getMonth()
{
  uint8_t bcd = ds1307_readRegister(DS1307_MONTH);
  bcd &= 0x1F; // Mask for month
  return bcd_to_dec(bcd);
}

uint8_t ds1307_getYear()
{
  uint8_t bcd = ds1307_readRegister(DS1307_YEAR);
  return bcd_to_dec(bcd);
}



void ds1307_setSeconds(uint8_t val)
{
  uint8_t bcd = dec_to_bcd(val);
  ds1307_writeRegister(DS1307_SECONDS, bcd);
}

void ds1307_setMinutes(uint8_t val)
{
  uint8_t bcd = dec_to_bcd(val);
  ds1307_writeRegister(DS1307_MINUTES, bcd);
}

void ds1307_setHours(uint8_t val)
{
  uint8_t bcd = dec_to_bcd(val);
  ds1307_writeRegister(DS1307_HOURS, bcd);
}

void ds1307_setWeekday(uint8_t val)
{
  uint8_t bcd = dec_to_bcd(val);
  ds1307_writeRegister(DS1307_WEEKDAY, bcd);
}

void ds1307_setDate(uint8_t val)
{
  uint8_t bcd = dec_to_bcd(val);
  ds1307_writeRegister(DS1307_DATE, bcd);
}

void ds1307_setMonth(uint8_t val)
{
  uint8_t bcd = dec_to_bcd(val);
  ds1307_writeRegister(DS1307_MONTH, bcd);
}

void ds1307_setYear(uint16_t val)
{
  uint8_t bcd = dec_to_bcd(val % 100);
  ds1307_writeRegister(DS1307_YEAR, bcd);
}