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
	enum Position{DOWN_POSITION = -1, INTAKE_POSITION = 5, CHEVAL_POSITION = 20, SHOOTING_POSITION = 60, MOVING};
	Arm(DigitalInput * encResetSwitch, Encoder * armEncoder, VictorSP * armMotor);
	void goToDown();
	void goToIntake();
	void goToCheval();
	void goToShooting();
	int targetPosition();
	bool isAtTargetPosition();
	void update();
	virtual ~Arm();
private:
	DigitalInput * resetSwitch;
	Encoder * encoder;
	VictorSP * arm;
	PIDController * armPID;
	Position target;
	Position position;

	static const int MARGIN_OF_ERROR = 10;

	static const int ARM_UP_SPEED = 1.0;
	static const int ARM_DOWN_SPEED = -0.5;
};

#endif /* SRC_ARM_H_ */
