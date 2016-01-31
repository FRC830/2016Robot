/*
 * Arm.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: RatPack
 */

#include <Arm.h>
#include <WPILib.h>
#include "../wpiutils/830utilities.h"

Arm::Arm(Encoder * armEncoder, VictorSP * armMotor) {
	encoder = armEncoder;
	arm = armMotor;
	targetPosition = INTAKE_POSITION;
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
}

Arm::~Arm() {}

