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
	bool goingToIntake = false;
	enum Position{DOWN_POSITION = 0, INTAKE_POSITION = 200, CHEVAL_POSITION = 2000, SHOOTING_POSITION = 2300};
	Arm(DigitalInput * encResetSwitch, Encoder * armEncoder, VictorSP * armMotor);
	void goToDown();
	void goToIntake();
	void goToShooting();
	void goToSwitch();
	void goToCheval();

	double targetPosition();
	bool isAtTargetPosition();
	double encoderValue();
	void setPID(double p, double i, double d);
	double pidSetpoint();

	bool bottomSwitchPressed();//used to be private

	void reset();
	void update();
	virtual ~Arm();
private:
	DigitalInput * resetSwitch;
	Encoder * encoder;
	VictorSP * arm;
	PIDController * armPID;
	bool goingToSwitch = false;

	static const int MARGIN_OF_ERROR = 50;
	//static const int ARM_UP_SPEED = 1.0;
	//static const int ARM_DOWN_SPEED = -0.5;
};

#endif /* SRC_ARM_H_ */
