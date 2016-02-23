/*
 * CameraFeeds.cpp
 *
 *  Created on: Feb 16, 2015
 *      Author: Mike Anderson
 */

#include <WPILib.h>
#include <Camera.h>
	//CAMERAFEEDS::CAMERAFEEDS(Joystick *newJoy)
	CAMERAFEEDS::CAMERAFEEDS(){
		int imaqError;
		imaqError = IMAQdxOpenCamera(camNameFront, IMAQdxCameraControlModeController, &camFront);
		if(imaqError != IMAQdxErrorSuccess) {
			DriverStation::ReportError("IMAQdxOpenCamera error: " + std::to_string((long)imaqError) + "\n");
		}
		imaqError = IMAQdxOpenCamera(camNameBack, IMAQdxCameraControlModeController, &camBack);
		if(imaqError != IMAQdxErrorSuccess) {
			DriverStation::ReportError("IMAQdxOpenCamera error: " + std::to_string((long)imaqError) + "\n");
		}

		curCam = camFront;
		frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
		server = CameraServer::GetInstance();
		server->SetQuality(imgQuality);
		//contrlr = newJoy;
	}

	CAMERAFEEDS::~CAMERAFEEDS() {

	}
	void CAMERAFEEDS::init() {
		changeCam(kBtCamFront);
	}

	void  CAMERAFEEDS::end() {
		IMAQdxStopAcquisition(curCam);
	}

	void CAMERAFEEDS::changeCam(int newId) {
		int imaqError;
		IMAQdxStopAcquisition(curCam);

		if (kBtCamFront == newId)
			curCam = camFront;
		else
			curCam = camBack;

		imaqError = IMAQdxConfigureGrab(curCam);
		if(imaqError != IMAQdxErrorSuccess) {
			DriverStation::ReportError("IMAQdxConfigureGrab error: " + std::to_string((long)imaqError) + "\n");
		}
		IMAQdxStartAcquisition(curCam);
		//curCam = newId;
	}

	void CAMERAFEEDS::updateCam() {
		int imaqError;
		imaqError = IMAQdxGrab(curCam, frame, true, NULL);
		if(imaqError != IMAQdxErrorSuccess) {
			DriverStation::ReportError("IMAQdxGrab error: " + std::to_string((long)imaqError) + "\n");
		}
		server->SetImage(frame);

	}

	void CAMERAFEEDS::run() {
		/*
		if (contrlr->GetRawButton(CAMERAFEEDS::kBtCamFront)) {
			changeCam(camFront);
		}
		if (contrlr->GetRawButton(CAMERAFEEDS::kBtCamBack)) {
			changeCam(camBack);
		}*/
		updateCam();
	}

