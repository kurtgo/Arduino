/*
 * rtc.cpp
 *
 *  Created on: Sep 14, 2016
 *      Author: kghome
 */

#include <RTC.h>
#include "Arduino.h"
#include <Wire.h>

#define DS3231_I2C_ADDRESS 0x68

// Convert normal decimal numbers to binary coded decimal
byte RTC::decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte RTC::bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}
void RTC::setup()
{
  Wire.begin();
  // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
  // setDS3231time(30,42,21,4,26,11,14);
}
void RTC::setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void RTC::readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
void RTC::displayTime(Print * out)
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // send it to the serial monitor
  out->print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  out->print(F(":"));
  if (minute<10)
  {
    out->print(F("0"));
  }
  out->print(minute, DEC);
  out->print(F(":"));
  if (second<10)
  {
    out->print(F("0"));
  }
  out->print(second, DEC);

  out->print(F(" "));

  switch(dayOfWeek){
  case 1:
    out->print(F("Sunday"));
    break;
  case 2:
    out->print(F("Monday"));
    break;
  case 3:
    out->print(F("Tuesday"));
    break;
  case 4:
    out->print(F("Wednesday"));
    break;
  case 5:
    out->print(F("Thursday"));
    break;
  case 6:
    out->print(F("Friday"));
    break;
  case 7:
    out->print(F("Saturday"));
    break;
  }
  out->print(F(", "));

  out->print(month, DEC);
  out->print(F("/"));
  out->print(dayOfMonth, DEC);
  out->print(F("/"));
  out->print(year+2000, DEC);
}

void RTC::updateTime()
{
	byte dow;
	readDS3231time(&second, &minute, &hour, &dow, &dayOfMonth, &month,
			&year);
	dayOfWeek = (DAYOFWEEK)dow;
}
int RTC::getHour()
{
	  // retrieve data from DS3231
	updateTime();
	return hour;
}
enum RTC::DAYOFWEEK RTC::getDow()
{
	return dayOfWeek;
}
RTC::RTC() {
	// TODO Auto-generated constructor stub

}

RTC::~RTC() {
	// TODO Auto-generated destructor stub
}

