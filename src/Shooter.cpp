/*
 * Shooter.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: RatPack
 */

#include <Shooter.h>
#include <WPILib.h>
#include "../wpiutils/830utilities.h"

Shooter::Shooter(DigitalInput * intakeSwitch, VictorSP * intakeMotor, VictorSP * shooterMotor) {
	lineBreak = intakeSwitch;
	intake = intakeMotor;
	shooter = shooterMotor;
	timer = new Timer();
	state = NONE;
}

void Shooter::rollIn(){
	state = ROLLING_IN;
}

void Shooter::rollOut(){
	state = ROLLING_OUT;
	timer->Reset();
}

void Shooter::stopRolling(){
	if(state == ROLLING_OUT){
		state = NONE;
	}
}

void Shooter::stopShooting(){
	if(state == SHOOTING){
		state = NONE;
	}
}

void Shooter::shoot(){
	state = SHOOTING;
}

void Shooter::update(){
	switch(state){
		case(ROLLING_IN):
			if(isBroken()){
				intake->Set(0.0);
				state = NONE;
			}
			else{
				intake->Set(1.0);
			}
			break;
		case(ROLLING_OUT):
			intake->Set(-1.0);
			break;
		case(SHOOTING):
			intake->Set(1.0);
			shooter->Set(1.0);
			break;
		case(NONE):
			intake->Set(0.0);
			shooter->Set(0.0);
	}
}

bool Shooter::isBroken(){
	return lineBreak->Get();
}

Shooter::~Shooter() {}


