/*
 * rtc.h
 *
 *  Created on: Sep 14, 2016
 *      Author: kghome
 */

#include "Print.h"
#ifndef LIBRARIES_RTC_RTC_H_
#define LIBRARIES_RTC_RTC_H_

typedef unsigned char byte;

class RTC {
public:
	RTC();
	virtual ~RTC();

	enum DAYOFWEEK {
		DAY_SUNDAY = 1,
		DAY_MONDAY,
		DAY_TUESDAY,
		DAY_WEDNESDAY,
		DAY_THURSDAY,
		DAY_FRIDAY,
		DAY_SATURDAY
	};

	byte decToBcd(byte val);
	byte bcdToDec(byte val);
	void setup();
	void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
	dayOfMonth, byte month, byte year);
	void readDS3231time(byte *second,
	byte *minute,
	byte *hour,
	byte *dayOfWeek,
	byte *dayOfMonth,
	byte *month,
	byte *year);
	void displayTime(Print *out);
	int getHour(void);
	enum DAYOFWEEK getDow(void);


protected:
    byte second, minute, hour, dayOfMonth, month, year;
    DAYOFWEEK dayOfWeek;
    void updateTime(void);

};

#endif /* LIBRARIES_RTC_RTC_H_ */
