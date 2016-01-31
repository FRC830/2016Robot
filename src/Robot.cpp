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

	static const int SHOOTER_VICTOR_PWM = 4;
	static const int INTAKE_VICTOR_PWM = 5;
	static const int ARM_VICTOR_PWM = 6;


	static const int ENCODER_1_DIO = 0;
	static const int ENCODER_2_DIO = 1;
	static const int INTAKE_DIO = 2;
	static const int RESET_DIO = 3;

	RobotDrive * drive;

	GamepadF310 * pilot;
	GamepadF310 * copilot;
	Shooter * shooter;
	Arm * arm;


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

		shooter = new Shooter(
				new VictorSP(SHOOTER_VICTOR_PWM),
				new VictorSP(INTAKE_VICTOR_PWM),
				new DigitalInput(INTAKE_DIO)
			);
		arm = new Arm(
				new Encoder(ENCODER_1_DIO, ENCODER_2_DIO),
				new VictorSP(ARM_VICTOR_PWM),
				new DigitalInput(RESET_DIO)
			);
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


		if (copilot-> ButtonState(F310Buttons::A)) {
			shooter -> rollIn();
		}else if (copilot -> ButtonState(F310Buttons::B)){
			shooter ->stop();
		}else if (copilot-> ButtonState(F310Buttons::X)){
			shooter-> rollOut();
		}else if (copilot->ButtonState(F310Buttons::Y)){
			shooter -> shoot();
		}

		if (copilot-> ){}

		shooter->update();
		arm ->update():
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
