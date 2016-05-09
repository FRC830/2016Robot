/*
 * RatTail.h
 *
 *  Created on: Feb 7, 2016
 *      Author: ratpack
 */

#ifndef SRC_RATTAIL_H_
#define SRC_RATTAIL_H_

#include <WPILib.h>

class RatTail {
public:
	RatTail(DigitalInput * bottom, PowerDistributionPanel * powerPanel, VictorSP * tailMotor);
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
	Timer * currentTimer;
	Timer * timer;
	PowerDistributionPanel * pdp;
	State state;
	Position position;

	static constexpr float UP_SPEED = 0.3;
	static constexpr float DOWN_SPEED = -0.45;
	static constexpr float UP_TIME = 2;
	static constexpr float DOWN_TIME = 1;

	static const int MOTOR_PDP_CHANNEL = 2;
	static constexpr float MAX_CURRENT = 2; //amps
	static constexpr float MAX_CURRENT_TIME = 0.2; //seconds
};


#endif /* SRC_RATTAIL_H_ */
