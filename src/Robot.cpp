#include "WPILib.h"
#include "../wpiutils/830utilities.h"

#include "Arm.h"
#include "Shooter.h"

class Robot: public IterativeRobot



{
private:

	static const int FRONT_LEFT_PWM = 0;
	static const int FRONT_RIGHT_PWM = 1;
	static const int BACK_LEFT_PWM = 2;
	static const int BACK_RIGHT_PWM = 3;

	RobotDrive * drive;

	GamepadF310 * pilot;
	GamepadF310 * copilot;

	void RobotInit()
	{
		drive = new RobotDrive(
			new VictorSP(FRONT_LEFT_PWM),
			new VictorSP(BACK_LEFT_PWM),
			new VictorSP(FRONT_RIGHT_PWM),
			new VictorSP(BACK_RIGHT_PWM)
		);

		pilot = new GamepadF310(0);
		copilot = new GamepadF310(1);
	}

	void AutonomousInit()
	{

	}

	void AutonomousPeriodic()
	{

	}

	void TeleopInit()
	{

	}

	void TeleopPeriodic()
	{
		float left = pilot->LeftY();
		float right = pilot->RightY();

		drive->TankDrive(left, right, true);

		shooter -> update();
		if copilot->
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
