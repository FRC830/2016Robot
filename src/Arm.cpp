/*
 * Arm.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: RatPack
 */

#include <Arm.h>

Arm::Arm(Encoder * armEncoder, VictorSP * armMotor) {
	encoder = armEncoder;
	arm = armMotor;
}

Arm::~Arm() {
	// TODO Auto-generated destructor stub
}

