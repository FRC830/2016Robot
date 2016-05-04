/*
 * RatTail.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: ratpack
 */

#include "RatTail.h"
#include <WPILib.h>
#include <Lib830.h>

RatTail::RatTail(DigitalInput * bottom, PowerDistributionPanel * powerPanel, VictorSP * tailMotor){
	motor = tailMotor;
	bottomSwitch = bottom;
	pdp = powerPanel;
	currentTimer = new Timer();
	timer = new Timer();
	state = STATIONARY;
	position = POS_TOP;
}

void RatTail::goToBottom(){
	if(state != TO_BOTTOM){
		state = TO_BOTTOM;
		timer->Reset();
		timer->Start();
	}
}

void RatTail::goToTop(){
	if(state != TO_TOP){
		state = TO_TOP;
		// used to enforce maximum time
		timer->Reset();
		timer->Start();
	}
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
void RatTail::update(){
	if(state != STATIONARY && pdp->GetCurrent(MOTOR_PDP_CHANNEL) > MAX_CURRENT){
		currentTimer->Start();
	}
	else {
		currentTimer->Stop();
		currentTimer->Reset();
	}
	switch(state){
		case TO_BOTTOM:
			if (atBottom()) {
				state = STATIONARY;
				break;
			}
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
			if(atTop()) {
				state = STATIONARY;
				break;
			}
			if(currentTimer->Get() < MAX_CURRENT_TIME && timer->Get() < UP_TIME){
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
