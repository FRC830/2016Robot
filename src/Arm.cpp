/*
 * Arm.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: RatPack
 */

#include <Arm.h>
#include <WPILib.h>
#include "../wpiutils/830utilities.h"

Arm::Arm(DigitalInput * encResetSwitch, Encoder * armEncoder, VictorSP * armMotor) {
	resetSwitch = encResetSwitch;
	encoder = armEncoder;
	arm = armMotor;
	targetPosition = INTAKE_POSITION;
}

void Arm::goToDown(){
	targetPosition = DOWN_POSITION;
}

void Arm::goToIntake(){
	targetPosition = INTAKE_POSITION;
}

void Arm::goToShooting(){
	targetPosition = SHOOTING_POSITION;
}

void Arm::update(){
	if( (encoder->Get() - targetPosition) > MARGIN_OF_ERROR){
		arm->Set(ARM_UP_SPEED);
	}
	else if( (encoder->Get() - targetPosition) < -MARGIN_OF_ERROR){
		arm->Set(ARM_DOWN_SPEED);
	}
	arm->Set(0.0);

	if(resetSwitch->Get())
		encoder->Reset();
}

Arm::~Arm() {}

