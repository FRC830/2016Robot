/*
 * Shooter.h
 *
 *  Created on: Jan 31, 2016
 *      Author: RatPack
 */

#ifndef SRC_SHOOTER_H_
#define SRC_SHOOTER_H_

#include "arm.h"
#include <WPILib.h>
#include "../wpiutils/830utilities.h"

class Shooter {

public:
	enum State{ROLLING_IN, ADJUSTING_ROLL_IN, ROLLING_OUT, STARTING_SHOOTER, SHOOTING, STATIONARY};
	Shooter(DigitalInput * lineBreak, VictorSP * intake, VictorSP * shooter, Arm * robotArm);
	void stop();//not for normal stopping
	void rollIn();
	void rollOut();
	void shoot();
	bool hasBall();
	void setShootWaitTime(double shootWaitTime);

	void reset();
	void update();
	virtual ~Shooter();
private:
	DigitalInput * lineBreak;
	VictorSP * intake;
	VictorSP * shooter;
	Arm * arm;
	Timer * timer;

	const float ROLL_OUT_SPEED = 1.0;
	const float ADJUST_SPEED = 0.1;
	const float ROLL_IN_SPEED = -0.6;
	const float ROLL_TO_SHOOT_SPEED = -1.0; //speed to intake when feeding ball from storage to shooter
	const double ROLL_OUT_TIME = 1.0;//time to roll out completely

	const float SHOOT_SPEED = 1.0;
	/*const*/ double SHOOT_WAIT_TIME = 0.4;//to wait to activate intake motor after activating shooter motor;
	const double SHOOT_TIME =  1.0;//time to stop shooting

	State state;
};

#endif /* SRC_SHOOTER_H_ */
