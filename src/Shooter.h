/*
 * Shooter.h
 *
 *  Created on: Jan 31, 2016
 *      Author: RatPack
 */

#ifndef SRC_SHOOTER_H_
#define SRC_SHOOTER_H_

#include <WPILib.h>
#include "../wpiutils/830utilities.h"

class Shooter {

public:
	enum State{ROLLING_IN, ROLLING_OUT, SHOOTING, NONE};
	Shooter(DigitalInput * lineBreak, VictorSP * intake, VictorSP * shooter);
	void rollIn();
	void rollOut();
	void stopRolling();//for stopping rolling after rolling out, not in
	void shoot();
	void stopShooting();
	void update();
	virtual ~Shooter();
private:
	DigitalInput * lineBreak;
	VictorSP * intake;
	VictorSP * shooter;
	Timer * timer;

	const double ROLL_OUT_TIME = 2.0;//time to roll out
	const double SHOOT_WAIT_TIME = 0.5;//to wait to activate intake motor after activating shooter motor;
	const double SHOOT_TIME =  2.0;//time to stop shooting

	bool isBroken();
	State state;
};

#endif /* SRC_SHOOTER_H_ */
