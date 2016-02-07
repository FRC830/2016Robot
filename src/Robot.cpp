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
	static const int TAIL_VICTOR_PWM = 7;

	static const int ENCODER_1_DIO = 0;
	static const int ENCODER_2_DIO = 1;
	static const int INTAKE_DIO = 2;
	static const int RESET_DIO = 3;
	static const int TAIL_BOTTOM_DIO = 4;
	static const int TAIL_TOP_DIO = 5;


	static const int TICKS_TO_FULL_SPEED = 100;

	RobotDrive * drive;

	GamepadF310 * pilot;
	GamepadF310 * copilot;
	Shooter * shooter;
	Arm * arm;
	RatTail * ratTail;

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

		arm = new Arm(
			new DigitalInput(RESET_DIO),
			new Encoder(ENCODER_1_DIO, ENCODER_2_DIO),
			new VictorSP(ARM_VICTOR_PWM)
		);
		shooter = new Shooter(
			new DigitalInput(INTAKE_DIO),
			new VictorSP(INTAKE_VICTOR_PWM),
			new VictorSP(SHOOTER_VICTOR_PWM),
			arm
		);
		ratTail = new RatTail(
			new DigitalInput(TAIL_BOTTOM_DIO),
			new DigitalInput(TAIL_TOP_DIO),
			new VictorSP(TAIL_VICTOR_PWM)
		);

		SmartDashboard::init();
	}

	void AutonomousInit()
	{

	}

	void AutonomousPeriodic()
	{

	}

	void TeleopInit()
	{
		arm->reset();
		shooter->reset();
	}

	float previousForward = 0;

	void TeleopPeriodic(){
		float targetForward = pilot->LeftY();
		float turn = pilot->RightY();

		float forward = accel(previousForward, targetForward, TICKS_TO_FULL_SPEED);

		drive->ArcadeDrive(forward, turn, true);

		previousForward = forward;

		if (copilot->ButtonState(F310Buttons::A)) { //gather a ball
			shooter->rollIn();
		}else if (copilot->ButtonState(F310Buttons::B)){ //cancel gathering a ball
			shooter->stop();
		}else if (copilot->ButtonState(F310Buttons::X)){ //eject a ball
			shooter-> rollOut();
		}else if (copilot->ButtonState(F310Buttons::Y)){ //shoot a ball
			shooter -> shoot();
		}else if (copilot->DPadY() == 1){
			ratTail->goToTop();
		}else if (copilot->DPadY() == -1){//for moving down after cheval
			ratTail->goToBottom();
		}

		SmartDashboard::PutBoolean("Has Ball", shooter->hasBall());
		SmartDashboard::PutNumber("Shooter Encoder: ", arm->encoderValue());

		double shootWaitTime = SmartDashboard::GetNumber("Shoot Wait Time:", 0.4);

		double p = SmartDashboard::GetNumber("P:", 0.1);
		double i = SmartDashboard::GetNumber("I:", 0.0);
		double d = SmartDashboard::GetNumber("D:", 0.0);
		arm->setPID(p, i, d);

		SmartDashboard::PutNumber("PID Setpoint:", arm->pidSetpoint());

		shooter->setShootWaitTime(shootWaitTime);

		shooter->update();
		arm->update();
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
