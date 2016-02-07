/*
 * RatTail.h
 *
 *  Created on: Feb 7, 2016
 *      Author: ratpack
 */

#ifndef SRC_RATTAIL_H_
#define SRC_RATTAIL_H_

#include <WPILib.h>
#include "../wpiutils/830utilities.h"

class RatTail {
public:
	RatTail(DigitalInput * bottom, DigitalInput * top, VictorSP * tailMotor);
	void goToTop();
	void goToBottom();
	void update();
private:
	enum Target{BOTTOM, TOP};
	VictorSP * motor;
	DigitalInput * bottomSwitch;
	DigitalInput * topSwitch;
	Target target;

	const double UP_SPEED = 0.5;
	const double DOWN_SPEED = 0.3;
};


#endif /* SRC_RATTAIL_H_ */
