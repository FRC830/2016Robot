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

	void reset();
	void update();
	virtual ~Shooter();
private:
	DigitalInput * lineBreak;
	VictorSP * intake;
	VictorSP * shooter;
	Arm * arm;
	Timer * timer;

	static constexpr float ROLL_OUT_SPEED = 1.0;
	static constexpr float ADJUST_SPEED = 0.3;
	static constexpr float ROLL_IN_SPEED = -0.5;
	static constexpr float ROLL_TO_SHOOT_SPEED = -1.0; //speed to intake when feeding ball from storage to shooter
	static constexpr float ROLL_OUT_TIME = 1.0;//time to roll out completely

	static constexpr float SHOOT_SPEED = 1.0;
	static constexpr float SHOOT_WAIT_TIME = 2.0;//to wait to activate intake motor after activating shooter motor;
	static constexpr float SHOOT_TIME = 1.5;//time to stop shooting

	State state;
};

#endif /* SRC_SHOOTER_H_ */
