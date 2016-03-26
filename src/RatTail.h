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
	bool atTop();
	bool atBottom();
	void update();
	bool topSwitchPressed();
	bool bottomSwitchPressed();
private:
	enum State{TO_BOTTOM, TO_TOP, STATIONARY};
	enum Position{POS_BOTTOM, POS_TOP, POS_MOVING};
	VictorSP * motor;
	DigitalInput * bottomSwitch;
	DigitalInput * topSwitch;
	Timer * timer;
	State state;
	Position position;

	static constexpr float UP_SPEED = 0.4;
	static constexpr float DOWN_SPEED = -0.3;
	static constexpr float UP_TIME = 1.2;
	static constexpr float DOWN_TIME = UP_TIME * -(UP_SPEED / DOWN_SPEED);
};


#endif /* SRC_RATTAIL_H_ */
