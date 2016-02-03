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
	armPID->SetSetpoint(DOWN_POSITION);

	armPID->Enable();
}

void Arm::goToDown(){
	armPID->SetSetpoint(DOWN_POSITION);
}

void Arm::goToCheval(){
	armPID->SetSetpoint(CHEVAL_POSITION);
}

void Arm::goToIntake(){
	armPID->SetSetpoint(INTAKE_POSITION);
}

void Arm::goToShooting(){
	armPID->SetSetpoint(SHOOTING_POSITION);
}

double Arm::targetPosition(){
	return armPID->GetSetpoint();
}

bool Arm::isAtTargetPosition(){
	return abs(armPID->GetError())<MARGIN_OF_ERROR;
}

bool Arm::bottomSwitchPressed(){
	return !resetSwitch->Get();
}

void Arm::update(){
	if(bottomSwitchPressed())
		encoder->Reset();
}

Arm::~Arm() {}

