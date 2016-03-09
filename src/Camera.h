/*
 * CameraFeeds.h
 *
 *  Created on: Feb 16, 2015
 *      Author: Mike Anderson
 */

#include "WPILib.h"
#include "../wpiutils/LCameraServer.h"

#ifndef SRC_CAMERAFEEDS_H_
#define SRC_CAMERAFEEDS_H_

class CameraFeeds {
public:

	const int kBtCamFront = 1;
	const int kBtCamBack = 2;
	//CAMERAFEEDS(Joystick *newJoy);
	CameraFeeds();
	~CameraFeeds();

	void init();
	void run();
	void end();
	void changeCam(int newId);
	void updateCam();

private:
	IMAQdxSession camFront;
	IMAQdxSession camBack;
	IMAQdxSession curCam;
	Image *frame;
	LCameraServer *server;
	const char *camNameFront = "cam0";
	const char *camNameBack = "cam1";
	int imgQuality = 60;
	//Joystick *contrlr;

};

#endif /* SRC_CAMERAFEEDS_H_ */

