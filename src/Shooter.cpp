/*
 * Shooter.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: RatPack
 */

#include <WPILib.h>
#include <Lib830.h>

#include "Shooter.h"
#include "Arm.h"

Shooter::Shooter(DigitalInput * intakeSwitch, VictorSP * intakeMotor, VictorSP * shooterMotor, Arm * robotArm)
:shoot_close(false),shoot_pos(0)
{
	lineBreak = intakeSwitch;
	intake = intakeMotor;
	shooter = shooterMotor;
	timer = new Timer();
	arm = robotArm;
	state = STATIONARY;
}

void Shooter::stop(){ //not for normal stopping
	state = STATIONARY;
	intake->Set(0.0);
	shooter->Set(0.0);
	timer->Stop();
	timer->Reset();
}

void Shooter::rollIn(){
	arm->goToIntake();
	state = ROLLING_IN;
}

void Shooter::rollOut(){
	arm->goToIntake();
	state = ROLLING_OUT;
	timer->Reset();
}

void Shooter::shoot(bool close) {
	state = STARTING_SHOOTER;
	shoot_close = close;
	shoot_pos=0;
	timer->Stop();
	timer->Reset();
	timer->Start();
}

void Shooter::shoot(int pos) {
	state = STARTING_SHOOTER;
	shoot_close =false;
	shoot_pos = pos;
	timer->Stop();
	timer->Reset();
	timer->Start();
}

void Shooter::reset(){
	state = STATIONARY;
	timer->Stop();
	timer->Reset();
}

void Shooter::update(){
	switch(state){
		case(ROLLING_IN):
			intake->Set(ROLL_IN_SPEED);
			if(hasBall()){
				intake->Set(0.0);
				state = ADJUSTING_ROLL_IN;
			}
			break;
		case(ADJUSTING_ROLL_IN):
			intake->Set(ADJUST_SPEED);
			if(!hasBall()){
				intake->Set(0.0);
				state = STATIONARY;
				timer->Stop();
				timer->Reset();
			}
			break;
		case(ROLLING_OUT):
			if(arm->isAtTargetPosition()){
				timer->Start();
				intake->Set(ROLL_OUT_SPEED);
				shooter->Set(-(SHOOT_SPEED / 2));
				if(timer->Get() >= ROLL_OUT_TIME){
					intake->Set(0.0);
					state = STATIONARY;
					timer->Stop();
					timer->Reset();
				}
			}
			break;
		case(STARTING_SHOOTER):
			shooter->Set(SHOOT_SPEED);
			if (shoot_close)
				arm->goToCloseShooting();
			else if (shoot_pos)
				arm->goToRaw(shoot_pos);
			else
				arm->goToShooting();
			if((timer->Get() >= SHOOT_WAIT_TIME) && (arm->isAtTargetPosition())){
				state = SHOOTING;
				intake->Set(ROLL_TO_SHOOT_SPEED);
				timer->Reset();
				timer->Start();
			}
			break;
		case(SHOOTING):
			intake->Set(ROLL_TO_SHOOT_SPEED);
			shooter->Set(SHOOT_SPEED);
			if(timer->Get() >= SHOOT_TIME){
				state = STATIONARY;
				arm->goToIntake();
				intake->Set(0.0);
				shooter->Set(0.0);
				timer->Stop();
				timer->Reset();
			}
			break;
		case(STATIONARY):
			intake->Set(0.0);
			shooter->Set(0.0);
			timer->Stop();
			timer->Reset();
			break;
	}
}

bool Shooter::hasBall(){
	return !lineBreak->Get();
}

Shooter::~Shooter() {}
