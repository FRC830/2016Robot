/*
 * Shooter.h
 *
 *  Created on: Jan 31, 2016
 *      Author: RatPack
 */

#ifndef SRC_SHOOTER_H_
#define SRC_SHOOTER_H_

#include <WPILib.h>
#include "arm.h"
#include "../wpiutils/830utilities.h"

class Shooter {

public:
	enum State{ROLLING_IN, ROLLING_OUT, STARTING_SHOOTER, SHOOTING, STATIONARY};
	Shooter(DigitalInput * lineBreak, VictorSP * intake, VictorSP * shooter, Arm * robotArm);
	void stop();//not for normal stopping
	void rollIn();
	void rollOut();
	void shoot();
	void update();
	bool hasBall();
	virtual ~Shooter();
private:
	DigitalInput * lineBreak;
	VictorSP * intake;
	VictorSP * shooter;
	Arm * arm;
	Timer * timer;

	const float ROLL_OUT_SPEED = 1.0;
	const float ROLL_IN_SPEED = -0.4;
	const float ROLL_TO_SHOOT_SPEED = -1.0; //speed to intake when feeding ball from storage to shooter
	const double ROLL_OUT_TIME = 2.0;//time to roll out completely

	const float SHOOT_SPEED = 1.0;
	const double SHOOT_WAIT_TIME = 0.5;//to wait to activate intake motor after activating shooter motor;
	const double SHOOT_TIME =  2.0;//time to stop shooting

	State state;
};

#endif /* SRC_SHOOTER_H_ */
