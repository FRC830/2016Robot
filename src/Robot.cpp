#include "WPILib.h"
#include "../wpiutils/830utilities.h"

#include "Timer.h"

#include "RobotDrive.h"

#include "Arm.h"
#include "Shooter.h"
#include "RatTail.h"
#include "Camera.h"

class Robot: public IterativeRobot
{
public:
	enum Obstacle{NOTHING, LOW_BAR, PORTCULLIS, CHEVAL_DE_FRISE, MOAT, RAMPARTS, DRAWBRIDGE, SALLYPORT, ROCK_WALL, ROUGH_TERRAIN};

private:

	static const int LEFT_PWM_ONE = 0;
	static const int LEFT_PWM_TWO = 4;
	static const int RIGHT_PWM_ONE = 1;
	static const int RIGHT_PWM_TWO = 3;

	static const int SHOOTER_VICTOR_PWM = 6;
	static const int INTAKE_VICTOR_PWM = 2;
	static const int ARM_VICTOR_PWM = 7;
	static const int TAIL_VICTOR_PWM = 5;
	static const int TEST_VICTOR = 8;

	static const int INTAKE_PDP_CHANNEL = 11;
	static constexpr float INTAKE_STOP_TIME = 2.0;

	static const int ENCODER_1_DIO = 0;
	static const int ENCODER_2_DIO = 1;
	static const int INTAKE_DIO = 2;

	static const int ARM_RESET_DIO = 3;
	static const int TAIL_BOTTOM_DIO = 4;
	//static const int TAIL_TOP_DIO = 5;

	static const int GEAR_SHIFT_SOL_FORWARD = 0;
	static const int GEAR_SHIFT_SOL_REVERSE = 1;
	static const DoubleSolenoid::Value LOW_GEAR = DoubleSolenoid::kForward;
	static const DoubleSolenoid::Value HIGH_GEAR = DoubleSolenoid::kReverse;

	static const int TICKS_TO_FULL_SPEED = 40;

	const int kCam0Button = 1;
	const int kCam1Button = 2;

	PowerDistributionPanel * pdp;
	Timer * intakeTimer;

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
	CameraFeeds * camerafeeds;
	//VictorSP * testVictor;

	SendableChooser * autonChooser;
	Obstacle autonObstacle;

	void arcadeDrive(float forward, float turn, bool squared = false){
		drive->ArcadeDrive(-forward, turn, squared);
	}

	void RobotInit()
	{
		autonChooser = new SendableChooser();
		drive = new RobotDrive(
			new VictorSP(LEFT_PWM_ONE),
			new VictorSP(LEFT_PWM_TWO),
			new VictorSP(RIGHT_PWM_ONE),
			new VictorSP(RIGHT_PWM_TWO)
		);

		pilot = new GamepadF310(0);
		copilot = new GamepadF310(1);

		arm = new Arm(
			new DigitalInput(ARM_RESET_DIO),
			new Encoder(ENCODER_1_DIO, ENCODER_2_DIO),
			new VictorSP(ARM_VICTOR_PWM)
		);

		pdp = new PowerDistributionPanel();
		intakeTimer = new Timer();
		shooter = new Shooter(
			new DigitalInput(INTAKE_DIO),
			new VictorSP(INTAKE_VICTOR_PWM),
			new VictorSP(SHOOTER_VICTOR_PWM),
			arm
		);
		ratTail = new RatTail(
			new DigitalInput(TAIL_BOTTOM_DIO),
			new VictorSP(TAIL_VICTOR_PWM)
		);
		gear_shift = new DoubleSolenoid(GEAR_SHIFT_SOL_FORWARD, GEAR_SHIFT_SOL_REVERSE);
		SmartDashboard::init();
		//testVictor = new VictorSP(TEST_VICTOR);

		camerafeeds = new CameraFeeds;

		camerafeeds->init();

		autonChooser = new SendableChooser();
		autonChooser->AddDefault("Low Bar", new Obstacle(LOW_BAR));
		autonChooser->AddObject("Do Nothing", new Obstacle(NOTHING));
		autonChooser->AddObject("Low Bar", new Obstacle(LOW_BAR));
		autonChooser->AddObject("Portcullis", new Obstacle(PORTCULLIS));
		autonChooser->AddObject("Cheval de Frise", new Obstacle(CHEVAL_DE_FRISE));
		autonChooser->AddObject("Moat", new Obstacle(MOAT));
		autonChooser->AddObject("Ramparts", new Obstacle(RAMPARTS));

		SmartDashboard::PutData("Auton Program", autonChooser);
	}

	void AutonomousInit()
	{
		timer->Reset();
		timer->Start();
		arm->goToSwitch();
		autonObstacle = *(Obstacle*)autonChooser->GetSelected();
	}

	void AutonomousPeriodic()
	{
		//where the robot starts on the field in relation to the obstacles
		//1 is the low bar, 5 is against the secret passage
		int location = 1;

		switch(autonObstacle){
			case LOW_BAR:
				//drive train
				if(timer->Get() < 4.0){
					arcadeDrive(0.75, 0.0, false);
				}
				else{
					arcadeDrive(0.0, 0.0);
				}
				break;
			case PORTCULLIS:
				//drive train?
				if(timer->Get() < 2){
					arm->goToSwitch();
					ratTail->goToBottom();
				}
				else if(timer->Get() < 4){
					arcadeDrive(0.75, 0.0, false);
				}
				else if(!ratTail->atTop()){
					ratTail->goToTop();
					timer->Reset();
					timer->Start();
				}
				else if(timer->Get() < 2){
					arcadeDrive(0.75, 0.0, false);
				}
				else{
					arcadeDrive(0.0, 0.0, false);
				}
				break;
			case CHEVAL_DE_FRISE:
				//rat tail?
				if(timer->Get() < 2){
					arcadeDrive(1.0, 0.0, false);
				}
				else if(!ratTail->atTop()){
					timer->Reset();
					timer->Start();
					ratTail->goToTop();
					arcadeDrive(0.0, 0.0, false);
				}
				else if(timer->Get() < 1){
					arcadeDrive(0.5, 0.0, false);
				}
				else{
					arcadeDrive(0.0, 0.0, false);
				}

				break;
			case MOAT:
				//drive train
				if(timer->Get() < 4){
					arcadeDrive(1.0, 0.0, false);
				}
				break;
			case RAMPARTS:
				//drive train
				if(timer->Get() < 4){
					arcadeDrive(1.0, 0.0, false);
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
					arcadeDrive(1.0, 0.0, false);
				}
				break;
			case ROUGH_TERRAIN:
				//drive train
				if(timer->Get() < 4){
					arcadeDrive(1.0, 0.0, false);
				}
				break;
			default:
				//Do Nothing
				arcadeDrive(0,0,false);
		}
		//shooting code
		switch(location){
		case 1:
			if(timer->Get() < 10){
				arcadeDrive(1.0, 0.0, false);
			}
			else if(timer->Get() < 15){
				shooter->shoot();
			}
			break;
		case 2:
			if(timer->Get() < 10){
				arcadeDrive(1.0, 0.0, false);
			}
			else if(timer->Get() < 15){
				shooter->shoot();
			}
			break;
		case 3:
			if(timer->Get() < 10){
				arcadeDrive(1.0, 0.0, false);
			}
			else if(timer->Get() < 15){
				shooter->shoot();
			}
			break;
		case 4:
			if(timer->Get() < 10){
				arcadeDrive(1.0, 0.0, false);
			}
			else if(timer->Get() < 15){
				shooter->shoot();
			}
			break;
		case 5:
			if(timer->Get() < 10){
				arcadeDrive(1.0, 0.0, false);
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
		arm->goToSwitch();
	}

	float previousForward = 0;

	void TeleopPeriodic(){
		float targetForward = pilot->LeftY();
		float turn = pilot->RightX()/1.5;

		float forward = accel(previousForward, targetForward, TICKS_TO_FULL_SPEED);

		previousForward = forward;

		arcadeDrive(forward, turn, true);

		if (pilot->ButtonState(F310Buttons::RightBumper)||pilot->ButtonState(F310Buttons::LeftBumper)){
			gear_shift->Set(LOW_GEAR);
		} else {
			gear_shift->Set(HIGH_GEAR);
		}
		if (pilot->ButtonState(F310Buttons::X)){
			arm->Cheval();
		} else if (pilot->ButtonState(F310Buttons::Y)){
			arm->goToDown();
		} else if (pilot->ButtonState(F310Buttons::A)){
			ratTail->goToTop();
		} else if (pilot->ButtonState(F310Buttons::B)){
			ratTail->goToBottom();
		}
		if (copilot->ButtonState(F310Buttons::A)) { //gather a ball
			shooter->rollIn();
		}else if (copilot->ButtonState(F310Buttons::B)){ //cancel gathering a ball
			shooter->stop();
		}else if (copilot->ButtonState(F310Buttons::X)){ //eject a ball
			shooter->rollOut();
		}else if (copilot->ButtonState(F310Buttons::Y)){ //shoot a ball
			shooter->shoot();
		}else if (copilot->DPadY() == 1){
			ratTail->goToTop();
		}else if (copilot->DPadY() == -1){//for moving down after cheval
			ratTail->goToBottom();
		}else if (copilot->ButtonState(F310Buttons::Back)){
			arm->goToSwitch();
		}

		if(pdp->GetCurrent(INTAKE_PDP_CHANNEL) > 10.0){
			intakeTimer->Start();
		}
		else{
			intakeTimer->Reset();
			intakeTimer->Stop();
		}
		if(intakeTimer->Get() > INTAKE_STOP_TIME){
			shooter->stop();
		}
		/*float testSpeed = 0;
		if (copilot->RightTrigger()> 0.05){
			testSpeed = copilot->RightTrigger()/3.0;
		}else if (copilot->LeftTrigger() > 0.05){
			testSpeed = -copilot->LeftTrigger()/3.0;
		}*/
		//testVictor->Set(testSpeed);

		//SmartDashboard::PutNumber("Test Speed", testSpeed);
		SmartDashboard::PutBoolean("Has Ball", shooter->hasBall());
		SmartDashboard::PutNumber("Arm Encoder: ", arm->encoderValue());
		SmartDashboard::PutBoolean("Arm Switch: ", arm->bottomSwitchPressed());
		SmartDashboard::PutBoolean("Rat tail switch", ratTail->switchPressed());

		/*
		SmartDashboard::PutNumber("P:",0.1);
		SmartDashboard::PutNumber("I:",0.1);
		SmartDashboard::PutNumber("D:",0.1); */
		double p = SmartDashboard::GetNumber("P:", 0.075);
		double i = SmartDashboard::GetNumber("I:", 15);
		double d = SmartDashboard::GetNumber("D:", 0.0);
		arm->setPID(p, i, d);

		SmartDashboard::PutNumber("PID Setpoint:", arm->pidSetpoint());

		shooter->update();
		arm->update();
		ratTail->update();
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
