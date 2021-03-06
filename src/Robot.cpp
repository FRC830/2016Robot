#include "WPILib.h"
#include "Lib830.h"

#include "Timer.h"

#include "RobotDrive.h"

#include "Arm.h"
#include "Shooter.h"
#include "RatTail.h"
#include "Camera.h"

using namespace Lib830;

class Robot: public IterativeRobot
{
public:
	enum Obstacle{NOTHING, TOUCH, LOW_BAR, LOW_BAR_2X, PORTCULLIS, CHEVAL_DE_FRISE, MOAT, RAMPARTS, DRAWBRIDGE, SALLYPORT, ROCK_WALL, ROUGH_TERRAIN};
	enum AutonPosition{NO_SHOOT, SHOOT1, SHOOT2, SHOOT3, SHOOT4, SHOOT5};
private:
	//PWM
	enum PWM_id {
		LEFT_PWM_ONE = 0,
		LEFT_PWM_TWO = 4,
		RIGHT_PWM_ONE = 1,
		RIGHT_PWM_TWO = 3,

		SHOOTER_VICTOR_PWM = 6,
		INTAKE_VICTOR_PWM = 2,
		ARM_VICTOR_PWM = 7,
		TAIL_VICTOR_PWM = 5,
		TEST_VICTOR = 8,
	};


	//Analog Input
	enum Analog_id {
		GYRO_ANALOG = 0,
	};

	//Digital IO
	enum DIO_id {
		ENCODER_1_DIO = 0,
		ENCODER_2_DIO = 1,
		INTAKE_DIO = 2,
		ARM_RESET_DIO = 3,
		TAIL_BOTTOM_DIO = 4,
		TAIL_TOP_DIO = 5,
		RANGE_PING_DIO = 6,
		RANGE_ECHO_DII = 7,
	};

	//Pneumatics Control Module output channels
	enum PCM_id {
		GEAR_SHIFT_SOL_FORWARD = 0,
		GEAR_SHIFT_SOL_REVERSE = 1,
	};
	static const DoubleSolenoid::Value LOW_GEAR = DoubleSolenoid::kForward;
	static const DoubleSolenoid::Value HIGH_GEAR = DoubleSolenoid::kReverse;

	//For handling intake motor burnout protection
	enum PDP_id {
		INTAKE_PDP_CHANNEL = 11,
	};
	static constexpr float INTAKE_STOP_TIME = 2.0;
	Timer * intakeTimer;

	static const int TICKS_TO_FULL_SPEED = 12;

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

	Lib830::AnalogGyro * gyro;
	// gyro angle compensation
	bool gyro_comp_active;
	Ultrasonic * range;

	PowerDistributionPanel * pdp;

	void arcadeDrive(float forward, float turn, bool squared = false){
		drive->ArcadeDrive(-forward, turn, squared);
	}

	void RobotInit()
	{
		if (GetWPILibVersion() < VersionInfo(2016, 5)) {
			DriverStation::ReportError("WPILib 2016 Update 5 or newer required");
			exit(1);
		}
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
			pdp,
			new VictorSP(TAIL_VICTOR_PWM)
		);
		gear_shift = new DoubleSolenoid(GEAR_SHIFT_SOL_FORWARD, GEAR_SHIFT_SOL_REVERSE);
		SmartDashboard::init();

		camerafeeds = new CameraFeeds;
		camerafeeds->init();

		gyro = new Lib830::AnalogGyro(GYRO_ANALOG);

		range = new Ultrasonic(
				new DigitalOutput(RANGE_PING_DIO),
				new DigitalInput(RANGE_ECHO_DII));

		autonChooser = new SendableChooser();
		autonChooser->AddDefault("Do Nothing", new Obstacle(NOTHING));
		autonChooser->AddObject("Touch obstacle", new Obstacle(TOUCH));
		autonChooser->AddObject("Low Bar", new Obstacle(LOW_BAR));
		autonChooser->AddObject("Low Bar 2x", new Obstacle(LOW_BAR_2X));
		autonChooser->AddObject("Portcullis", new Obstacle(PORTCULLIS));
		autonChooser->AddObject("Cheval de Frise", new Obstacle(CHEVAL_DE_FRISE));
		autonChooser->AddObject("Moat", new Obstacle(MOAT));
		autonChooser->AddObject("Ramparts", new Obstacle(RAMPARTS));
		autonChooser->AddObject("Rock Wall", new Obstacle(ROCK_WALL));
		autonChooser->AddObject("Rough Terrain", new Obstacle(ROUGH_TERRAIN));


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

	bool auton_stopped;
	void AutonArcadeDrive(float speed, float max_time, float delay_time = 0) {
		WARN_COND_CHANGE(abs(gyro->GetAngle()) >= 12, "excessive gyro drift: " << gyro->GetAngle());
		float time = timer->Get();
		if (time >= max_time || abs(gyro->GetAngle()) >= 20)
			auton_stopped = true;
		WARN_COND_ON(auton_stopped, "stopped: angle=" << gyro->GetAngle() <<
			" time=" << time << "/" << max_time);

		if (auton_stopped || time < delay_time) {
			arcadeDrive(0, 0);
		}
		else {
			float turn = gyro->GetAngle() / -15.0;
			arcadeDrive(speed, turn);
		}

	}

	void AutonomousInit()
	{
		puts("starting auton");
		printf("obstacle: %i\n", autonObstacle);
		auton_stopped = false;
		timer->Reset();
		timer->Start();
		gyro->Reset();
		arm->goToSwitch();
		gear_shift->Set(LOW_GEAR);
	}

	void AutonomousPeriodic()
	{
		SmartDashboard::PutData("gyro",gyro);
		//where the robot starts on the field in relation to the obstacles
		//1 is the low bar, 5 is against the secret passage
		//int location = 1;
		// float turn;
		// //int state = 1;
		// turn = gyro->GetAngle()/-15.0;


		float time = timer->Get();
		float turn = gyro->GetAngle() / -15.0;
		//WARN_COND_CHANGE(abs(gyro->GetAngle()) >= 15, "excessive gyro drift");
		bool hold_arm = false;
		switch(autonObstacle){
			case TOUCH:
				AutonArcadeDrive(0.25, 5);
				arm->goToCheval();
				break;

			case LOW_BAR:
			case PORTCULLIS:
				// delay for 1 second to allow rat tail to go down
				if (autonObstacle == LOW_BAR)
					AutonArcadeDrive(0.45, 4, 1);
				else
					AutonArcadeDrive(0.25, 9, 1);

				if (time < 1)
					hold_arm = true;

				if (time < 3)
					ratTail->goToBottom();
				else if (time > 13)
					ratTail->goToTop();
				break;

			case LOW_BAR_2X:
				if (1 < time && time < 4)
					AutonArcadeDrive(0.45, 15);
				else if (4.5 < time && time < 7.5)
					AutonArcadeDrive(-0.35, 15);

				if (time < 1)
					hold_arm = true;

				if (time > 12)
					ratTail->goToTop();
				else
					ratTail->goToBottom();
				break;

			case CHEVAL_DE_FRISE:
				//rat tail?
				if (time<4){
					AutonArcadeDrive(0.2, 10000);
					ratTail ->goToTop();
				}
				else if(time>=4 and time<5.5) {
					arcadeDrive(0,0);
					ratTail->goToBottom();
				}
				else if(time>=5.5 and time<7) {
					AutonArcadeDrive(0.7, 10000);
				}
				else {
					arcadeDrive(0,0);
					ratTail->goToTop();
				}
				break;

			case MOAT:
			case RAMPARTS:
			case ROCK_WALL:
			case ROUGH_TERRAIN:
				//drive train
				AutonArcadeDrive(0.8, 2);
				break;

			case SALLYPORT:
			case DRAWBRIDGE:
			default:
				//Do Nothing
				arcadeDrive(0,0);
				break;
		}
		ratTail->update();
		if (!hold_arm)
			arm->update();
	}

	void TeleopInit()
	{
		puts("teleop init");
		arm->reset();
		shooter->reset();
		arm->goToSwitch();
		SmartDashboard::PutNumber("custom shoot", SmartDashboard::GetNumber("custom shoot", Arm::CLOSE_SHOOTING_POSITION));
	}

	float previousForward = 0;
	void TeleopPeriodic(){
		//Drive Train
		float targetForward = pilot->LeftY();
		/*if (targetForward > 0.75) {
			targetForward = 0.75;
		}
		if (targetForward < -0.75) {
			targetForward = -0.75;
		} */
		float turn;
		if (pilot->RightY() > 0.9) {
			// compensate with gyro!
			if (!gyro_comp_active) {
				gyro->Reset();
				gyro_comp_active = true;
			}
			turn = -gyro->GetAngle() * 0.1;
		}
		else {
			gyro_comp_active = false;
			turn = pilot->RightX()/1.8;
		}
		SmartDashboard::PutNumber("turn",turn);
		if (pilot->RightTrigger() > 0.99) {
			turn = 0;
			targetForward = 0.3;
		}

		float forward = accel(previousForward, targetForward, TICKS_TO_FULL_SPEED);

		previousForward = forward;

		arcadeDrive(forward, turn, true);

		//Pilot controls
		if (pilot->LeftTrigger() > 0.5){
			gear_shift->Set(LOW_GEAR);
		} else {
			gear_shift->Set(HIGH_GEAR);
		}
		SmartDashboard::PutString("gear", gear_shift->Get() == LOW_GEAR ? "low" : "high");

		if (pilot->ButtonState(GamepadF310::BUTTON_X)){
			arm->goToDown();
		} else if (pilot->ButtonState(GamepadF310::BUTTON_Y)){
			arm->goToCheval();
		}

		//Copilot Controls
		if (copilot->ButtonState(GamepadF310::BUTTON_A)) { //gather a ball
			shooter->rollIn();
		}else if (copilot->ButtonState(GamepadF310::BUTTON_B)){ //cancel gathering a ball
			shooter->stop();
		}else if (copilot->ButtonState(GamepadF310::BUTTON_X)){ //eject a ball
			shooter->rollOut();
		}else if (copilot->ButtonState(GamepadF310::BUTTON_Y)){ //shoot a ball
			shooter->shoot();
			copilot->GamepadF310::RumbleLeft(1);
			copilot->GamepadF310::RumbleRight(1);
		}else if (copilot->ButtonState(GamepadF310::BUTTON_START)){// close shooting
			shooter->shoot(true);
		}else if (copilot->RightTrigger() > 0.9) {//custom shooting
			shooter->shoot((int)SmartDashboard::GetNumber("custom shoot", Arm::CLOSE_SHOOTING_POSITION));
		}else if (copilot->ButtonState(GamepadF310::BUTTON_BACK)){
			arm->goToIntake();
		}
		else if (copilot->ButtonState(GamepadF310::BUTTON_LEFT_BUMPER)) {
			arm->goToShooting();
		}
		else {
			copilot->GamepadF310::RumbleLeft(0);
			copilot->GamepadF310::RumbleRight(0);
		}

		if (copilot->DPadY() == -1 || pilot->ButtonState(GamepadF310::BUTTON_B)){//for moving down after cheval
			ratTail->goToBottom();
		} else {
			ratTail->goToTop();
		}
		//Automatically stop the intake motor if it pulls too much current for too long
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
		SmartDashboard::PutBoolean("Rat tail bottom switch", ratTail->bottomSwitchPressed());

		SmartDashboard::PutNumber("Rat Tail Current",pdp->GetCurrent(2));
		SmartDashboard::PutNumber("If arm is at Target", arm -> isAtTargetPosition());
		SmartDashboard::PutNumber("Going to Intake", arm->goingToIntake);
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

	void DisabledInit() {
		puts("disabled");
		gyro->Reset();
	}

	void DisabledPeriodic() {
		autonObstacle = autonChooser->GetSelected() ? *(Obstacle*)autonChooser->GetSelected() : NOTHING;
		shooterStatus = shooterChoice->GetSelected() ? *(AutonPosition*)shooterChoice->GetSelected() : NO_SHOOT;
		SmartDashboard::PutNumber("auton obstacle ID", autonObstacle);
		CameraPeriodic();
		SmartDashboard::PutData("gyro", gyro);
	}

	void CameraPeriodic() {
		if (pilot->DPadUp()) {
			camerafeeds-> changeCam(camerafeeds->kBtCamFront);
		}
		if (pilot->DPadDown()) {
			camerafeeds-> changeCam(camerafeeds->kBtCamBack);
		}

		camerafeeds->run();
	}

};

START_ROBOT_CLASS(Robot)
