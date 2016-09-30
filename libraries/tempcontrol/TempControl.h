/*
 * TempControl.h
 *
 *  Created on: Sep 14, 2016
 *      Author: kghome
 */

#ifndef LIBRARIES_TEMPCONTROL_TEMPCONTROL_H_
#define LIBRARIES_TEMPCONTROL_TEMPCONTROL_H_
#include "IRremote2.h"

#define SLOTS 5
class TempControl {
public:
	TempControl();
	virtual ~TempControl();

	void setTemp(float temp) { desiredtemp = temp; curstate=RESTART;}
	void checkTemp(int curtemp);
	enum FANSTATE {
		RESTART,
		OFF,
		LOW_SPEED,
		NORMAL_SPEED,
		HIGH_SPEED,
	};
	enum FANSTATE getstate() { return curstate; }
	void setstate(enum FANSTATE state, int curtemp);
	void setWorktime(int wk) {worktime = wk;}
	float updateTemp(float temp, int humidity);
	void reset();


protected:
	IRsend2 irsend;
	float desiredtemp;
	float lasttemp[SLOTS];
	int cur;
	int worktime;
	float average;
	int first_time;
	enum FANSTATE curstate;

};

#endif /* LIBRARIES_TEMPCONTROL_TEMPCONTROL_H_ */