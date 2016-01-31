/*
 * Shooter.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: RatPack
 */

#include <Shooter.h>
#include <WPILib.h>
#include "../wpiutils/830utilities.h"

Shooter::Shooter(DigitalInput * intakeSwitch, VictorSP * intakeMotor, VictorSP * shooterMotor, Arm * robotArm) {
	lineBreak = intakeSwitch;
	intake = intakeMotor;
	shooter = shooterMotor;
	timer = new Timer();
	arm = robotArm;

	state = NONE;
}

void Shooter::stop(){//not for normal stopping
	state = NONE;
	intake->Set(0.0);
	shooter->Set(0.0);
	timer->Stop();
	timer->Reset();
}

void Shooter::rollIn(){
	state = ROLLING_IN;
}

void Shooter::rollOut(){
	state = ROLLING_OUT;
	timer->Reset();
}

void Shooter::shoot(){
	state = STARTING_SHOOTER;
	timer->Reset();
	timer->Start();
}

void Shooter::update(){
	switch(state){
		case(ROLLING_IN):
			if(isBroken()){
				intake->Set(0.0);
				state = NONE;
				timer->Stop();
				timer->Reset();
			}
			else{
				intake->Set(1.0);
			}
			break;
		case(ROLLING_OUT):
			intake->Set(-1.0);
			if(timer->Get() >= ROLL_OUT_TIME){
				state = NONE;
				intake->Set(0.0);
				timer->Stop();
				timer->Reset();
			}
			break;
		case(STARTING_SHOOTER):
			shooter->Set(1.0);
			if(timer->Get() >= SHOOT_WAIT_TIME){
				state = SHOOTING;
				intake->Set(1.0);
				timer->Reset();
				timer->Start();
			}
			break;
		case(SHOOTING):
			intake->Set(1.0);
			shooter->Set(1.0);
			if(timer->Get() >= SHOOT_TIME){
				state = NONE;
				intake->Set(0.0);
				shooter->Set(0.0);
				timer->Stop();
				timer->Reset();
			}
			break;
		case(NONE):
			intake->Set(0.0);
			shooter->Set(0.0);
			timer->Stop();
			timer->Reset();
			break;
	}
}

bool Shooter::isBroken(){
	return lineBreak->Get();
}

Shooter::~Shooter() {}


