/*
 * Arm.h
 *
 *  Created on: Jan 28, 2016
 *      Author: RatPack
 */

#ifndef SRC_ARM_H_
#define SRC_ARM_H_

#include <WPILib.h>
#include "../wpiutils/830utilities.h"

class Arm {
public:
	Arm(Encoder * armEncoder, VictorSP * armMotor);
	void goToIntake();
	void goToShooting();
	void update();
	virtual ~Arm();
private:
	Encoder * encoder;
	VictorSP * arm;
	int targetPosition;

	static const int INTAKE_POSITION = 0;
	static const int SHOOTING_POSITION = 60;
	static const int MARGIN_OF_ERROR = 1;
	static const int ARM_UP_SPEED = 1.0;
	static const int ARM_DOWN_SPEED = -0.5;
};

#endif /* SRC_ARM_H_ */
