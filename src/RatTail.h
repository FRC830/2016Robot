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
	RatTail(DigitalInput * bottom, VictorSP * tailMotor);
	void goToTop();
	void goToBottom();
	bool atTop();
	bool atBottom();
	void update();
private:
	enum State{CHECK_BOTTOM, BOTTOM, TOP, STATIONARY};
	enum Position{BOTTOM, TOP, MOVING};
	VictorSP * motor;
	DigitalInput * bottomSwitch;
	Timer * timer;
	State state;
	Position position;

	static constexpr float UP_SPEED = 0.5;
	static constexpr float DOWN_SPEED = 0.3;
	static constexpr float UP_TIME = 1;
};


#endif /* SRC_RATTAIL_H_ */
