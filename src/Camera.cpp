/*
 * CameraFeeds.cpp
 *
 *  Created on: Feb 16, 2015
 *      Author: Mike Anderson
 */

#include <WPILib.h>
#include <Camera.h>
//CAMERAFEEDS::CAMERAFEEDS(Joystick *newJoy)
CameraFeeds::CameraFeeds(){
	int imaqError;
	imaqError = IMAQdxOpenCamera(camNameFront, IMAQdxCameraControlModeController, &camFront);
	if(imaqError != IMAQdxErrorSuccess) {
		DriverStation::ReportError("IMAQdxOpenCamera error: " + std::to_string(imaqError) + "\n");
	}
	imaqError = IMAQdxOpenCamera(camNameBack, IMAQdxCameraControlModeController, &camBack);
	if(imaqError != IMAQdxErrorSuccess) {
		DriverStation::ReportError("IMAQdxOpenCamera error: " + std::to_string(imaqError) + "\n");
	}

	curCam = camFront;
	frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
	server = CameraServer::GetInstance();
	server->SetQuality(imgQuality);
	//contrlr = newJoy;
}

CameraFeeds::~CameraFeeds() {

}
void CameraFeeds::init() {
	changeCam(kBtCamFront);
}

void CameraFeeds::end() {
	IMAQdxStopAcquisition(curCam);
}

void CameraFeeds::changeCam(int newId) {
	int imaqError;
	IMAQdxStopAcquisition(curCam);

	if (kBtCamFront == newId)
		curCam = camFront;
	else
		curCam = camBack;

	imaqError = IMAQdxConfigureGrab(curCam);
	if(imaqError != IMAQdxErrorSuccess) {
		DriverStation::ReportError("IMAQdxConfigureGrab error: " + std::to_string(imaqError) + "\n");
	}
	IMAQdxStartAcquisition(curCam);
	//curCam = newId;
}

void CameraFeeds::updateCam() {
	int imaqError = IMAQdxGrab(curCam, frame, true, NULL);
	if(imaqError != IMAQdxErrorSuccess) {
		DriverStation::ReportError("IMAQdxGrab error: " + std::to_string(imaqError) + "\n");
	}
	int dimx, dimy;
	imaqGetImageSize(frame, &dimx, &dimy);
	static const float color = 0x00ff00;
	static const float color2 = 0x0000ff;
	imaqDrawLineOnImage(frame, frame, IMAQ_DRAW_VALUE, {dimx/2, 0}, {dimx/2, dimy}, color);
	imaqDrawLineOnImage(frame, frame, IMAQ_DRAW_VALUE, {0, dimy/2}, {dimx, dimy/2}, color);
	imaqDrawLineOnImage(frame, frame, IMAQ_DRAW_VALUE, {dimx/2 + 2, 0}, {dimx/2 + 2, dimy}, color2);
	imaqDrawLineOnImage(frame, frame, IMAQ_DRAW_VALUE, {0, dimy/2 + 2}, {dimx, dimy/2 + 2}, color2);
	imaqDrawLineOnImage(frame, frame, IMAQ_DRAW_VALUE, {dimx/2 - 2, 0}, {dimx/2 - 2, dimy}, color2);
	imaqDrawLineOnImage(frame, frame, IMAQ_DRAW_VALUE, {0, dimy/2 - 2}, {dimx, dimy/2 - 2}, color2);
//	DrawTextOptions opts = {
//			"Arial",
//			12,
//			0,0,0,0,
//			IMAQ_LEFT,
//			FontColor(0x0000ff)
//	};
//	int font_used;
//	imaqDrawTextOnImage(frame, frame, {0,0}, curCam == camFront ? "front" : "back", &opts, &font_used);
	server->SetImage(frame);

}

void CameraFeeds::run() {
	/*
	if (contrlr->GetRawButton(CAMERAFEEDS::kBtCamFront)) {
		changeCam(camFront);
	}
	if (contrlr->GetRawButton(CAMERAFEEDS::kBtCamBack)) {
		changeCam(camBack);
	}*/
	updateCam();
}

