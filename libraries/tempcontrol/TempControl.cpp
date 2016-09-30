/*
 * TempControl.cpp
 *
 *  Created on: Sep 14, 2016
 *      Author: kghome
 */

#include <TempControl.h>
#include <arduino.h>
#include "IRremote2.h"
TempControl::TempControl() {

	worktime = 0;
	average = 0;
	first_time = 1;
}

TempControl::~TempControl() {
	// TODO Auto-generated destructor stub
}

void TempControl::reset()
{
	curstate = RESTART;
}
void TempControl::setstate(enum FANSTATE state, int curtemp)
{

	if (state != curstate) {
		// Timing sensitive, so disable interrupts for transmission
		noInterrupts();
		switch(state) {
		case RESTART:
			break;  // should not get here
		case HIGH_SPEED:
			irsend.sendHvacMitsubishi(HVAC_COLD, desiredtemp-5, FAN_SPEED_4, VANNE_AUTO_MOVE, false);
			break;
		case NORMAL_SPEED:
			irsend.sendHvacMitsubishi(HVAC_COLD, desiredtemp-3, FAN_SPEED_2, VANNE_H1, false);
			break;
		case LOW_SPEED:
			irsend.sendHvacMitsubishi(HVAC_COLD, desiredtemp, FAN_SPEED_1, VANNE_H1, false);
			break;
		case OFF:
			irsend.sendHvacMitsubishi(HVAC_COLD, desiredtemp-5, FAN_SPEED_AUTO, VANNE_AUTO_MOVE, true);
			break;
		}
		interrupts();
	}
	curstate = state;
}

float TempControl::updateTemp(float ctemp, int humidity)
{
	int i;
	if (first_time) {
		for (i=0;i<SLOTS;++i)
			lasttemp[i] = ctemp;
		first_time=0;
	}

	lasttemp[cur] = ctemp;
	cur = (cur + 1) % SLOTS;
	average = 0;
	for (i=0;i<SLOTS;++i)
		average += lasttemp[i];

	float curtemp = average/SLOTS;
	float stdev = 0;

	for (i=0;i<SLOTS;++i) {
		stdev += square(lasttemp[i]-curtemp);
	}
	stdev = stdev / SLOTS;
	stdev = sqrt(stdev);

	Serial.print(F("avg:"));
	Serial.print(curtemp);
	Serial.print(F(" stdev:"));
	Serial.print(stdev);
	Serial.print(F(" "));

	if (desiredtemp >= curtemp && !worktime) {
		Serial.print("OFF, after hours");
		setstate(OFF,desiredtemp);
	} else
	if ((curtemp + .5) < desiredtemp) {
		Serial.print(F("OFF-COLD"));
		setstate(OFF,desiredtemp);
	} else
	if (curtemp  < desiredtemp) {
		Serial.print(F("LOW"));
		setstate(LOW_SPEED,desiredtemp);
	} else
	if ((curtemp - .5) > desiredtemp) {
		// if we are way off..  then something is wrong, keep trying to set the state
		// by forcing the state to off.
		if (curtemp - 2 > desiredtemp)
			curstate=OFF;
		Serial.print(F("HI"));
		setstate(HIGH_SPEED,desiredtemp);
	} else {
		Serial.print(F("NORM"));
		setstate(LOW_SPEED,desiredtemp);
	}

	return desiredtemp;
}
