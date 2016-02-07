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
	target = TOP;
}

void RatTail::goToBottom(){
	target = BOTTOM;
}

void RatTail::goToTop(){
	target = TOP;
}

void RatTail::update(){
	switch(target){
		case BOTTOM:
			if(!bottomSwitch->Get()){
				motor->Set(DOWN_SPEED);
			}
			else{
				motor->Set(0);
			}
			break;
		case TOP:
			if(!topSwitch->Get()){
				motor->Set(UP_SPEED);
			}
			else{
				motor->Set(0);
			}
			break;
	}
}
