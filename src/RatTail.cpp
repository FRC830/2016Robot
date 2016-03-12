/*
 * RatTail.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: ratpack
 */

#include <RatTail.h>
#include <WPILib.h>
#include "../wpiutils/830utilities.h"

RatTail::RatTail(DigitalInput * bottom, VictorSP * tailMotor){
	motor = tailMotor;
	bottomSwitch = bottom;
	timer = new Timer();
	state = STATIONARY;
	position = POS_TOP;
}

void RatTail::goToBottom(){
	state = BOTTOM;
	timer->Reset();
	timer->Start();
}

void RatTail::goToTop(){
	// CHECK_BOTTOM moves to the bottom, then the top
	// don't do this unnecessarily
	if (position == POS_TOP)
		return;
	state = CHECK_BOTTOM;
	// used to enforce maximum time
	timer->Reset();
	timer->Start();
}

bool RatTail::atTop(){
	return position == POS_TOP;
}

bool RatTail::atBottom(){
	return position == POS_BOTTOM;
}
bool RatTail::switchPressed(){
	return !bottomSwitch->Get();
}
void RatTail::update(){
	switch(state){
		case BOTTOM:
			if(!switchPressed() && timer->Get() < DOWN_TIME){
				motor->Set(DOWN_SPEED);
				position = POS_MOVING;
			}
			else{
				motor->Set(0);
				state = STATIONARY;
				position = POS_BOTTOM;
			}
			break;
		case CHECK_BOTTOM:
			// moving to the bottom, then to the top
			if(!switchPressed() && timer->Get() < DOWN_TIME){
				motor->Set(DOWN_SPEED);
				position = POS_MOVING;
			}
			else{
				motor->Set(0);
				// start moving up again
				state = TOP;
				timer->Stop();
				timer->Reset();
				timer->Start();
			}
			break;
		case TOP:
			if(timer->Get() <= UP_TIME){
				motor->Set(UP_SPEED);
			}
			else{
				motor->Set(0);
				position = POS_TOP;
				timer->Stop();
				timer->Reset();
				state = STATIONARY;
			}
			break;
		case STATIONARY:
			motor->Set(0);
			timer->Stop();
			timer->Reset();
			break;
	}
}
