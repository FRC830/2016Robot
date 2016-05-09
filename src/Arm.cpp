/*
 * Arm.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: RatPack
 */

#include <Arm.h>
#include <WPILib.h>
#include <cmath>

using namespace std;

Arm::Arm(DigitalInput * encResetSwitch, Encoder * armEncoder, VictorSP * armMotor) {
	resetSwitch = encResetSwitch;
	encoder = armEncoder;
	arm = armMotor;
	armPID = new PIDController(0.075, 15, 0.0, armEncoder, armMotor);
	armPID->SetInputRange(-100,50000);
	armPID->SetOutputRange(-0.12, 0.4);
	armPID->SetAbsoluteTolerance(500);
	armPID->SetSetpoint(DOWN_POSITION);

	armPID->Enable();
}

void Arm::goToRaw(int pos){
	armPID->Enable();
	goingToSwitch = false;
	armPID->SetSetpoint(pos);
}

void Arm::goToDown(){
	armPID->Enable();
	goingToSwitch = false;
	armPID->SetSetpoint(DOWN_POSITION);
}

void Arm::goToIntake(){
	armPID->Enable();
	goingToSwitch = false;
	armPID->SetSetpoint(DOWN_POSITION);
	goingToIntake = true;
}

void Arm::goToShooting(){
	armPID->Enable();
	goingToSwitch = false;
	armPID->SetSetpoint(SHOOTING_POSITION);
}

void Arm::goToCloseShooting() {
	armPID->Enable();
	goingToSwitch = false;
	armPID->SetSetpoint(CLOSE_SHOOTING_POSITION);
}

void Arm::goToSwitch(){
	armPID->SetSetpoint(DOWN_POSITION);
	armPID->Disable();
	goingToSwitch = true;
}

void Arm::goToCheval(){
	armPID->SetSetpoint(CHEVAL_POSITION);
}

double Arm::targetPosition(){
	return armPID->GetSetpoint();
}

bool Arm::isAtTargetPosition(){
	return abs(armPID->GetError()) < MARGIN_OF_ERROR;
	//return true;
}

bool Arm::bottomSwitchPressed(){
	return resetSwitch->Get();
}

double Arm::encoderValue(){
	return encoder->Get();
}

void Arm::setPID(double p, double i, double d){
	armPID->SetPID(p, i, d);
	armPID->Enable();
}

void Arm::reset(){
	armPID->SetSetpoint(DOWN_POSITION);
}

double Arm::pidSetpoint(){
	return armPID->GetSetpoint();
}

void Arm::update(){
	if(bottomSwitchPressed()){
		encoder->Reset();
		if(goingToIntake){
			armPID->SetSetpoint(INTAKE_POSITION);
			goingToIntake = false;
		}
		if(goingToSwitch){
			armPID->Enable();
			goingToSwitch = false;
		}
	}
	if(goingToSwitch){
		arm->Set(-0.4);
	}
	if(!goingToSwitch){
		if(isAtTargetPosition()){
			armPID->Disable();
		}
		else{
			armPID->Enable();
		}
	}
}

Arm::~Arm() {}

