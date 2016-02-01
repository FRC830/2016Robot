/*
 * Arm.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: RatPack
 */

#include <Arm.h>
#include <WPILib.h>
#include "../wpiutils/830utilities.h"
#include <cmath> using namespace std

Arm::Arm(DigitalInput * encResetSwitch, Encoder * armEncoder, VictorSP * armMotor) {
	resetSwitch = encResetSwitch;
	encoder = armEncoder;
	arm = armMotor;
	armPID = new PIDController(0.1, 0, 0, armEncoder, armMotor);
	armPID->SetInputRange(0,10000);
	armPID->SetOutputRange(-1,1);

	target = INTAKE_POSITION;
	position = MOVING;
}

void Arm::goToDown(){
	target = DOWN_POSITION;
}

void Arm::goToCheval(){
	target = CHEVAL_POSITION;
}

void Arm::goToIntake(){
	target = INTAKE_POSITION;
}

void Arm::goToShooting(){
	target = SHOOTING_POSITION;
}

int Arm::targetPosition(){
	return target;
}

bool Arm::isAtTargetPosition(){
	//return position == target;
	return abs(armPID->GetError())<MARGIN_OF_ERROR;
}

void Arm::update(){
	/*
	if( (encoder->Get() - targetPosition) > MARGIN_OF_ERROR){
		arm->Set(ARM_UP_SPEED);
		position = MOVING;
	}
	else if( (encoder->Get() - targetPosition) < -MARGIN_OF_ERROR){
		arm->Set(ARM_DOWN_SPEED);
		position = MOVING;
	}
	else{
		arm->Set(0.0);
		position = targetPosition;
	}
	*/
	armPID->SetSetpoint(target);
	if(resetSwitch->Get())
		encoder->Reset();
}

Arm::~Arm() {}

