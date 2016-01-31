/*
 * Arm.h
 *
 *  Created on: Jan 28, 2016
 *      Author: RatPack
 */

#ifndef SRC_ARM_H_
#define SRC_ARM_H_

class Arm {
public:

	Arm(Encoder * armEncoder, VictorSP * armMotor);
	virtual ~Arm();
private:
	Encoder * encoder;
	VictorSP * arm;

	static const int INTAKE_POSITION = 0;
	static const int SHOOT_POSITION = 60;
};

#endif /* SRC_ARM_H_ */
