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
	enum Obstacle{NOTHING, TOUCH, LOW_BAR, PORTCULLIS, CHEVAL_DE_FRISE, MOAT, RAMPARTS, DRAWBRIDGE, SALLYPORT, ROCK_WALL, ROUGH_TERRAIN};
	enum AutonPosition{NO_SHOOT, SHOOT1, SHOOT2, SHOOT3, SHOOT4, SHOOT5};
private:
	//PWM
	static const int LEFT_PWM_ONE = 0;
	static const int LEFT_PWM_TWO = 4;
	static const int RIGHT_PWM_ONE = 1;
	static const int RIGHT_PWM_TWO = 3;

	static const int SHOOTER_VICTOR_PWM = 6;
	static const int INTAKE_VICTOR_PWM = 2;
	static const int ARM_VICTOR_PWM = 7;
	static const int TAIL_VICTOR_PWM = 5;
	static const int TEST_VICTOR = 8;

	//Analog Input
	static const int GYRO_ANALOG = 0;

	//Digital IO
	static const int ENCODER_1_DIO = 0;
	static const int ENCODER_2_DIO = 1;
	static const int INTAKE_DIO = 2;
	static const int ARM_RESET_DIO = 3;
	static const int TAIL_BOTTOM_DIO = 4;
	static const int RANGE_PING_DIO = 6;
	static const int RANGE_ECHO_DII = 7;

	//Pneumatics Control Module output channels
	static const int GEAR_SHIFT_SOL_FORWARD = 0;
	static const int GEAR_SHIFT_SOL_REVERSE = 1;
	static const DoubleSolenoid::Value LOW_GEAR = DoubleSolenoid::kForward;
	static const DoubleSolenoid::Value HIGH_GEAR = DoubleSolenoid::kReverse;

	//For handling intake motor burnout protection
	static const int INTAKE_PDP_CHANNEL = 11;
	static constexpr float INTAKE_STOP_TIME = 2.0;
	Timer * intakeTimer;

	static const int TICKS_TO_FULL_SPEED = 15;

	const int kCam0Button = 1;
	const int kCam1Button = 2;
	CameraFeeds * camerafeeds;

	GamepadF310 * pilot;
	GamepadF310 * copilot;

	RobotDrive * drive;
	VictorSP * frontLeft;
	VictorSP * backLeft;
	VictorSP * frontRight;
	VictorSP * backRight;
	DoubleSolenoid * gear_shift;

	Shooter * shooter;
	Arm * arm;
	RatTail * ratTail;

	Timer * timer;
	SendableChooser * autonChooser;
	Obstacle autonObstacle;
	SendableChooser * shooterChoice;
	AutonPosition shooterStatus;

	AnalogGyro * gyro;
	// gyro angle compensation
	bool gyro_comp_active;
	Ultrasonic * range;

	PowerDistributionPanel * pdp;

	void arcadeDrive(float forward, float turn, bool squared = false){
		drive->ArcadeDrive(-forward, turn, squared);
	}

	void RobotInit()
	{
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
		timer = new Timer();
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

		camerafeeds = new CameraFeeds;
		camerafeeds->init();

		gyro = new AnalogGyro(GYRO_ANALOG);

		range = new Ultrasonic(
				new DigitalOutput(RANGE_PING_DIO),
				new DigitalInput(RANGE_ECHO_DII));

		autonChooser = new SendableChooser();
		autonChooser->AddDefault("Do Nothing", new Obstacle(NOTHING));
		autonChooser->AddObject("Touch obstacle", new Obstacle(TOUCH));
		autonChooser->AddObject("Low Bar", new Obstacle(LOW_BAR));
		autonChooser->AddObject("Portcullis", new Obstacle(PORTCULLIS));
		autonChooser->AddObject("Cheval de Frise", new Obstacle(CHEVAL_DE_FRISE));
		autonChooser->AddObject("Moat", new Obstacle(MOAT));
		autonChooser->AddObject("Ramparts", new Obstacle(RAMPARTS));

		SmartDashboard::PutData("Auton Program", autonChooser);

		shooterChoice = new SendableChooser();
		shooterChoice->AddObject("Nothing", new AutonPosition(NO_SHOOT));
		shooterChoice->AddObject("Position 1", new AutonPosition(SHOOT1));
		shooterChoice->AddObject("Position 2", new AutonPosition(SHOOT2));
		shooterChoice->AddObject("Position 3", new AutonPosition(SHOOT3));
		shooterChoice->AddObject("Position 4", new AutonPosition(SHOOT4));
		shooterChoice->AddObject("Position 5", new AutonPosition(SHOOT5));

		SmartDashboard::PutData("Auton shooter", shooterChoice);
	}

	void AutonomousInit()
	{
		timer->Reset();
		timer->Start();
		gyro->Reset();
		arm->goToSwitch();
		gear_shift->Set(LOW_GEAR);
	}

	void AutonomousPeriodic()
	{
		//where the robot starts on the field in relation to the obstacles
		//1 is the low bar, 5 is against the secret passage
		//int location = 1;
		float turn;
		//int state = 1;
		turn = gyro->GetAngle()/-15.0;
		SmartDashboard::PutNumber("Gyro angle", gyro->GetAngle());
		SmartDashboard::PutNumber("Turn angle", turn);

		switch(autonObstacle){
			case TOUCH:
			case LOW_BAR:
			{
				float speed, time;
				if (autonObstacle == TOUCH) {
					speed = 0.25;
					time = 5;
				}
				else { // LOW_BAR
					speed = 0.5;
					time = 3;
					arm->goToSwitch();
				}
				//drive train
				if(timer->Get() < time){
					ratTail->goToBottom();
					arcadeDrive(speed, turn, false);
				}
				else{
					arcadeDrive(0.0,0.0);
				}
				break;
			}
			case PORTCULLIS:
				//drive train?
				if(timer->Get() < 2){
					arm->goToSwitch();
					ratTail->goToBottom();
				}
				else if(timer->Get() < 4){
					arcadeDrive(0.75, turn, false);
				}
				else if(!ratTail->atTop()){
					ratTail->goToTop();
				}
				else if(timer->Get() < 12){
					arcadeDrive(0.75, turn, false);
				}
				else{
					arcadeDrive(0.0,0.0);
				}
				break;
			case CHEVAL_DE_FRISE:
				//rat tail?
				if(timer->Get() < 2){
					arm->goToCheval();
					arcadeDrive(0.5, turn, false);
				}
				else if(timer->Get() < 4){
					arcadeDrive(0.0,0.0);
					arm->goToDown();
				}
				else if(timer->Get() < 5){
					arcadeDrive(0.25, turn, false);
					arm->goToCheval();
				}
				else if(timer->Get() < 8){
					arcadeDrive(0.75, turn, false);
				}
				else{
					arcadeDrive(0.0,0.0);
				}

				break;
			case MOAT:
				//drive train
				if(timer->Get() < 2){
					arcadeDrive(0.8, 0.0, false);
				}
				else{
					arcadeDrive(0.0,0.0);
				}
				break;
			case RAMPARTS:
				//drive train
				if(timer->Get() < 2){
					arcadeDrive(0.8, 0.0, false);
				}
				else{
					arcadeDrive(0.0,0.0);
				}
				break;
			case SALLYPORT:
				//nothing for now...maybe never
				arcadeDrive(0.0,0.0);
				break;
			case DRAWBRIDGE:
				//still nothing-arm?
				arcadeDrive(0.0,0.0);
				break;
			case ROCK_WALL:
				//drive train
				if(timer->Get() < 2){
					arcadeDrive(0.8, 0.0, false);
				}
				else{
					arcadeDrive(0.0,0.0);
				}
				break;
			case ROUGH_TERRAIN:
				//drive train
				if(timer->Get() < 2){
					arcadeDrive(0.8, 0.0, false);
				}
				else{
					arcadeDrive(0.0, 0.0);
				}
				break;
			default:
				//Do Nothing
				arcadeDrive(0,0,false);
		}
		ratTail->update();
		arm->update();
	}

	void TeleopInit()
	{
		arm->reset();
		shooter->reset();
		arm->goToSwitch();
	}

	float previousForward = 0;
	void TeleopPeriodic(){
		//Drive Train
		float targetForward = pilot->LeftY();
		float turn;
		if (pilot->ButtonState(F310Buttons::LeftStick)) {
			// compensate with gyro!
			if (!gyro_comp_active) {
				gyro->Reset();
				gyro_comp_active = true;
			}
			turn = -gyro->GetAngle() * 0.1;
		}
		else {
			gyro_comp_active = false;
			turn = pilot->RightX()/1.5;
		}
		SmartDashboard::PutNumber("turn",turn);
		float forward = accel(previousForward, targetForward, TICKS_TO_FULL_SPEED);

		previousForward = forward;

		arcadeDrive(forward, turn, true);

		//Pilot controls
		if (pilot->ButtonState(F310Buttons::RightBumper)||pilot->ButtonState(F310Buttons::LeftBumper)){
			gear_shift->Set(LOW_GEAR);
		} else {
			gear_shift->Set(HIGH_GEAR);
		}
		SmartDashboard::PutString("gear", gear_shift->Get() == LOW_GEAR ? "low" : "high");
		if (pilot->ButtonState(F310Buttons::X)){
			arm->goToCheval();
		} else if (pilot->ButtonState(F310Buttons::Y)){
			arm->goToDown();
		} else if (pilot->ButtonState(F310Buttons::A)){
			ratTail->goToTop();
		} else if (pilot->ButtonState(F310Buttons::B)){
			ratTail->goToBottom();
		}

		//Copilot Controls
		if (copilot->ButtonState(F310Buttons::A)) { //gather a ball
			shooter->rollIn();
		}else if (copilot->ButtonState(F310Buttons::B)){ //cancel gathering a ball
			shooter->stop();
		}else if (copilot->ButtonState(F310Buttons::X)){ //eject a ball
			shooter->rollOut();
		}else if (copilot->ButtonState(F310Buttons::Y)){ //shoot a ball
			shooter->shoot();
		}else if (copilot->ButtonState(F310Buttons::Start)){// close shooting
			shooter->shoot(true);
		}else if (copilot->DPadY() == 1){
			ratTail->goToTop();
		}else if (copilot->DPadY() == -1){//for moving down after cheval
			ratTail->goToBottom();
		}else if (copilot->ButtonState(F310Buttons::Back)){
			arm->goToSwitch();
		}

		//Autoomatically stop the intake motor if it pulls too much current for too long
		//prevents melting and fires and such
		SmartDashboard::PutNumber("Intake Current", pdp->GetCurrent(INTAKE_PDP_CHANNEL));
		if(pdp->GetCurrent(INTAKE_PDP_CHANNEL) > 10.0){
			intakeTimer->Start();
		} else {
			intakeTimer->Reset();
			intakeTimer->Stop();
		}
		if(intakeTimer->Get() > INTAKE_STOP_TIME){
			shooter->stop();
		}

		SmartDashboard::PutNumber("Gyro Angle", gyro->GetAngle());
		SmartDashboard::PutBoolean("Has Ball", shooter->hasBall());
		SmartDashboard::PutNumber("Arm Encoder: ", arm->encoderValue());
		SmartDashboard::PutBoolean("Arm Switch: ", arm->bottomSwitchPressed());
		SmartDashboard::PutBoolean("Rat tail switch", ratTail->switchPressed());

		/*
		SmartDashboard::PutNumber("P:",0.1);
		SmartDashboard::PutNumber("I:",0.1);
		SmartDashboard::PutNumber("D:",0.1); */
		/*double p = SmartDashboard::GetNumber("P:", 0.075);
		double i = SmartDashboard::GetNumber("I:", 15);
		double d = SmartDashboard::GetNumber("D:", 0.0);
		arm->setPID(p, i, d);*/

		SmartDashboard::PutNumber("PID Setpoint:", arm->pidSetpoint());

		shooter->update();
		arm->update();
		ratTail->update();

		//SmartDashboard::PutNumber("Range in inches", range->GetRangeInches());
		//range->SetAutomaticMode(true);

		CameraPeriodic();
	}

	void TestPeriodic()
	{

	}

	void DisabledPeriodic() {
		autonObstacle = autonChooser->GetSelected() ? *(Obstacle*)autonChooser->GetSelected() : NOTHING;
		shooterStatus = shooterChoice->GetSelected() ? *(AutonPosition*)shooterChoice->GetSelected() : NO_SHOOT;
		SmartDashboard::PutNumber("auton obstacle ID", autonObstacle);
		CameraPeriodic();
	}

	void CameraPeriodic() {
		if (pilot->ButtonState(F310Buttons::DPadUp)) {
			camerafeeds-> changeCam(camerafeeds->kBtCamFront);
		}
		if (pilot->ButtonState(F310Buttons::DPadDown)){
			camerafeeds-> changeCam(camerafeeds->kBtCamBack);
		}

		camerafeeds->run();
	}

};

START_ROBOT_CLASS(Robot)
