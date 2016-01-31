/*
 * Arm.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: RatPack
 */

#include <Arm.h>

Arm::Arm(int pwm) {
	arm = new VictorSP(pwm);
}

Arm::~Arm() {
	// TODO Auto-generated destructor stub
}

