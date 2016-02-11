#include "WPILib.h"
#include "../wpiutils/830utilities.h"

#include "Timer.h"

#include "RobotDrive.h"

#include "Arm.h"
#include "Shooter.h"
#include "RatTail.h"

class Robot: public IterativeRobot
{
private:

	static const int FRONT_LEFT_PWM = 0;//this pwm setup does not work with the gearbox
	static const int FRONT_RIGHT_PWM = 1;//the front and back wheels can't be controlled separately
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

	static const int GEAR_SHIFT_SOL_FORWARD = 0;
	static const int GEAR_SHIFT_SOL_REVERSE = 1;
	static const DoubleSolenoid::Value LOW_GEAR = DoubleSolenoid::kForward;
	static const DoubleSolenoid::Value HIGH_GEAR = DoubleSolenoid::kReverse;

	static const int TICKS_TO_FULL_SPEED = 100;

	enum Obstacle{LOW_BAR, PORTCULLIS, CHEVAL_DE_FRISE, MOAT, RAMPARTS, DRAWBRIDGE, SALLYPORT, ROCK_WALL, ROUGH_TERRAIN};

	RobotDrive * drive;

	Timer * timer;

	DoubleSolenoid * gear_shift;

	VictorSP * frontLeft;
	VictorSP * backLeft;
	VictorSP * frontRight;
	VictorSP * backRight;

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
		gear_shift = new DoubleSolenoid(GEAR_SHIFT_SOL_FORWARD, GEAR_SHIFT_SOL_REVERSE);
		SmartDashboard::init();
	}

	void AutonomousInit()
	{
		timer->Reset();
		timer->Start();
	}

	void AutonomousPeriodic()
	{
		//where the robot starts on the field in relation to the obstacles
		//1 is the low bar, 5 is against the secret passage
		int location = 1;

		Obstacle obstacle = LOW_BAR;

		switch(obstacle){
			case LOW_BAR:
				//drive train
				if(timer->Get() < 4.0){
					drive->ArcadeDrive(1.0, 0.0, false);
				}
				break;
			case PORTCULLIS:
				//drive train?
				if(timer->Get() < 4){
					drive->ArcadeDrive(1.0, 0.0, false);
				}
				break;
			case CHEVAL_DE_FRISE:
				//rat tail?
				if(timer->Get() < 2){
					drive->ArcadeDrive(1.0, 0.0, false);
				}
				else if(timer->Get() < 5){
					ratTail->goToTop();
					drive->ArcadeDrive(1.0, 0.0, false);
				}
				break;
			case MOAT:
				//drive train
				if(timer->Get() < 4){
					drive->ArcadeDrive(1.0, 0.0, false);
				}
				break;
			case RAMPARTS:
				//drive train
				if(timer->Get() < 4){
					drive->ArcadeDrive(1.0, 0.0, false);
				}
				break;
			case SALLYPORT:
				//nothing for now...maybe never
				break;
			case DRAWBRIDGE:
				//still nothing-arm?
				break;
			case ROCK_WALL:
				//drive train
				if(timer->Get() < 4){
					drive->ArcadeDrive(1.0, 0.0, false);
				}
				break;
			case ROUGH_TERRAIN:
				//drive train
				if(timer->Get() < 4){
					drive->ArcadeDrive(1.0, 0.0, false);
				}
				break;
		}
		//shooting code
		switch(location){
		case 1:
			if(timer->Get() < 10){
				drive->ArcadeDrive(1.0, 0.0, false);
			}
			else if(timer->Get() < 15){
				shooter->shoot();
			}
			break;
		case 2:
			if(timer->Get() < 10){
				drive->ArcadeDrive(1.0, 0.0, false);
			}
			else if(timer->Get() < 15){
				shooter->shoot();
			}
			break;
		case 3:
			if(timer->Get() < 10){
				drive->ArcadeDrive(1.0, 0.0, false);
			}
			else if(timer->Get() < 15){
				shooter->shoot();
			}
			break;
		case 4:
			if(timer->Get() < 10){
				drive->ArcadeDrive(1.0, 0.0, false);
			}
			else if(timer->Get() < 15){
				shooter->shoot();
			}
			break;
		case 5:
			if(timer->Get() < 10){
				drive->ArcadeDrive(1.0, 0.0, false);
			}
			else if(timer->Get() < 15){
				shooter->shoot();
			}
			break;
		}
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

		if (pilot->ButtonState(F310Buttons::RightBumper)||pilot->ButtonState(F310Buttons::LeftBumper)){
			gear_shift->Set(LOW_GEAR);
		} else {
			gear_shift->Set(HIGH_GEAR);
		}

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

		double p = SmartDashboard::GetNumber("P:", -0.1);
		double i = SmartDashboard::GetNumber("I:", 12.5);
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
