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
	state = BOTTOM;
	position = POS_MOVING;
}

void RatTail::goToBottom(){
	state = BOTTOM;
}

void RatTail::goToTop(){
	state = CHECK_BOTTOM;
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
			if(!switchPressed()){
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
			if(!switchPressed()){
				motor->Set(DOWN_SPEED);
				position = POS_MOVING;
			}
			else{
				motor->Set(0);
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
