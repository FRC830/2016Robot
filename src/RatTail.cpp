/*
 * RatTail.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: ratpack
 */

#include <RatTail.h>
#include <WPILib.h>
#include "../wpiutils/830utilities.h"

RatTail::RatTail(DigitalInput * bottom, DigitalInput * top, VictorSP * tailMotor){
	motor = tailMotor;
	bottomSwitch = bottom;
	topSwitch = top;
	timer = new Timer();
	state = STATIONARY;
	position = POS_TOP;
}

void RatTail::goToBottom(){
	state = TO_BOTTOM;
	timer->Reset();
	timer->Start();
}

void RatTail::goToTop(){
	state = TO_TOP;
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
bool RatTail::bottomSwitchPressed(){
	return !bottomSwitch->Get();
}
bool RatTail::topSwitchPressed(){
	return !topSwitch->Get();
}
void RatTail::update(){
	switch(state){
		case TO_BOTTOM:
			if(!bottomSwitchPressed() && timer->Get() < DOWN_TIME){
				motor->Set(DOWN_SPEED);
				position = POS_MOVING;
			}
			else{
				motor->Set(0);
				state = STATIONARY;
				position = POS_BOTTOM;
			}
			break;
		case TO_TOP:
			if(!topSwitchPressed() && timer->Get() < UP_TIME){
				motor->Set(UP_SPEED);
				position = POS_MOVING;
			}
			else{
				motor->Set(0);
				position = POS_TOP;
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
