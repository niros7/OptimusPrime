/*
 * HamsterUtilities.cpp
 *
 *  Created on: May 19, 2017
 *      Author: user
 */

#include "HamsterUtilities.h"
#include <HamsterAPIClientCPP/Hamster.h>
#include <iostream>
#include <string>
#include <math.h>
#include "LocalizationManager.h"
#include "Location.h"
#include "ConfigFile.h"

using namespace std;
using namespace HamsterAPI;

const int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };


HamsterUtilities::HamsterUtilities(Hamster* hamster, double hamsterRadius,
							       cv::Mat map, double resolution, LocalizationManager* locManager,
								   OccupancyGrid navMap) {
	ConfigFile *conf;
	conf = ConfigFile::GetInstance();

	this->map = map;
	this->width = map.cols;
	this->height = map.rows;
	this->resolution = resolution;
	this->hamsterRadius = hamsterRadius;
	this->wayPoints = new Location[70];
	this->hamster = hamster;
	this->locManager = locManager;
	this->navMap = navMap;


	this->mapCopy = cv::Mat(width, height,CV_8UC3);

	Location l = { conf->startX, conf->startY, -2.1 , 2.1 , conf->startYAW};

	this->currLoc = l ;

	this->prevLoc = currLoc;
}

void HamsterUtilities::blowMap(OccupancyGrid ogrid)
{
	// Blowing the map
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				if (ogrid.getCell(i, j) == CELL_FREE)
						{
							blowSurroundings(i, j, WHITE);
						}
		}

		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				if (ogrid.getCell(i, j) == CELL_OCCUPIED)
					{
						blowSurroundings(i, j, BLACK);
					}
				else if ( ogrid.getCell(i, j) == CELL_UNKNOWN)
					{
						paintCell(i,j, GRAY);
					}
		}

}

void HamsterUtilities::blowSurroundings(int x, int y, int pixelColor) {
	int blowingRadius = ceil(this->hamsterRadius / this->resolution);

	for (int i = x - blowingRadius; i <= x + blowingRadius; ++i) {
		if (i < 0 || i > this->height) continue;

		for (int j = y - blowingRadius; j <= y + blowingRadius; ++j) {
			if (j < 0 || j > this->width) continue;

			paintCell(i,j, pixelColor);

			if (pixelColor == this->WHITE)
			{
				navMap.setCell(i, j, CELL_FREE);
			}
			else
			{
				navMap.setCell(i, j, CELL_OCCUPIED);
			}
		}
	}

}

double HamsterUtilities::distance(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void HamsterUtilities::paintCell(int x, int y, int color)
{

	switch (color)
	{
	case BLACK:
		map.at<cv::Vec3b>(x,y)[0] = 0;
		map.at<cv::Vec3b>(x,y)[1] = 0;
		map.at<cv::Vec3b>(x,y)[2] = 0;
		break;
	case WHITE:
		map.at<cv::Vec3b>(x,y)[0] = 255;
		map.at<cv::Vec3b>(x,y)[1] = 255;
		map.at<cv::Vec3b>(x,y)[2] = 255;
		break;
	case BLUE:
		map.at<cv::Vec3b>(x,y)[0] = 255;
		map.at<cv::Vec3b>(x,y)[1] = 0;
		map.at<cv::Vec3b>(x,y)[2] = 0;
		break;
	case GREEN:
		map.at<cv::Vec3b>(x,y)[0] = 0;
		map.at<cv::Vec3b>(x,y)[1] = 255;
		map.at<cv::Vec3b>(x,y)[2] = 0;
		break;
	case RED:
		map.at<cv::Vec3b>(x,y)[0] = 0;
		map.at<cv::Vec3b>(x,y)[1] = 0;
		map.at<cv::Vec3b>(x,y)[2] = 255;
		break;
	case YELLOW:
		map.at<cv::Vec3b>(x,y)[0] = 0;
		map.at<cv::Vec3b>(x,y)[1] = 255;
		map.at<cv::Vec3b>(x,y)[2] = 255;
		break;
	case GRAY:
		map.at<cv::Vec3b>(x,y)[0] = 169;
		map.at<cv::Vec3b>(x,y)[1] = 169;
		map.at<cv::Vec3b>(x,y)[2] = 169;
		break;
	}

}


// This method paints the road AND creates the wayPoints, return number of way points
int HamsterUtilities::paintRoad(int StartX, int StartY, string route)
{
	if (route.length() > 0)
	{

		int dir; char c;
		int prevDir = -1;
		int x = StartX;
		int y = StartY;
		float wayDone = 0.0; // Way done since last way Point
		int numberOfWayPoints = 0;

		paintCell(x, y, BLUE);

		for (unsigned int i = 0; i<route.length(); i++)
		{
			c = route.at(i);
			dir = atoi(&c);

			x = x + dx[dir];
			y = y + dy[dir];

			if (prevDir != dir &&
				prevDir != -1)
			{
				wayPoints[numberOfWayPoints].Xpos = x;
				wayPoints[numberOfWayPoints].Ypos = y;
				numberOfWayPoints++;

				paintCell(x, y, YELLOW);
				wayDone = 0.0;
			}
			else
			{
				if (dir % 2 == 1 ) // Going diagnal, pixel is reso*sqrt(2)
					wayDone += (resolution * sqrt(2));
				else
					wayDone += resolution;

				if (wayDone >= resolution*1000) // if greater than 1 METER and no wayPoint, declare one
				{
					wayPoints[numberOfWayPoints].Xpos = x;
					wayPoints[numberOfWayPoints].Ypos = y;
					numberOfWayPoints++;


					paintCell(x, y, YELLOW);
					wayDone = 0.0;
				}
				else
				{
					paintCell(x, y, RED);
					wayDone += (resolution * sqrt(2));
				}

			}

			prevDir = dir;
		}

		wayPoints[numberOfWayPoints].Xpos = x;
		wayPoints[numberOfWayPoints].Ypos = y;
		numberOfWayPoints++;

		paintCell(x, y, GREEN);


		for(int i = 0; i < 1024 ; i++)
		{
			for(int j = 0; j < 1024; j++)
				{
					mapCopy.at<cv::Vec3b>(i,j)[0] = map.at<cv::Vec3b>(i,j)[0];
					mapCopy.at<cv::Vec3b>(i,j)[1] = map.at<cv::Vec3b>(i,j)[1];
					mapCopy.at<cv::Vec3b>(i,j)[2] = map.at<cv::Vec3b>(i,j)[2];
				}
		}

		return numberOfWayPoints;
	}



	return 0;
}

void HamsterUtilities::updatePose()
{
	prevLoc = currLoc;

/*
	currLoc = locManager->getMaxBeliefParticle();
	currLoc.Yaw += 120;
	if (currLoc.Yaw > 360)
		currLoc.Yaw -= 360;
*/

	Pose p = hamster->getPose();
	currLoc.Xpos = (double)(470) - p.getX() / this->resolution;
	currLoc.Ypos = (double)(470) - p.getY() / this->resolution;
	currLoc.Yaw = p.getHeading() + 120;

	if (currLoc.Yaw < 0)
		currLoc.Yaw += 360;
}

void HamsterUtilities::MoveToNextWayPoint(int index)
{
	int i = 0;

	cout << "Waypoint #" << index << endl;

	int destX = wayPoints[index].Xpos;
	int destY = wayPoints[index].Ypos;

	double angleToNext = atan2(destY - currLoc.Ypos, destX - currLoc.Xpos);
	angleToNext = angleToNext*180/M_PI - 30;

	if(angleToNext < 0)
		angleToNext += 360;

	// Turn around
	while (abs(currLoc.Yaw - angleToNext) > angleTolerange )
	{
		float diff = currLoc.Yaw - angleToNext;
		int direction;

		if ((diff > 0 && diff < 180) ||
		 	 diff < -180)
			// Going right
			direction = -1;
		else
			// Going Left
			direction = 1;

		hamster->sendSpeed(0.08, turningAngle * direction);

	//	locManager->updateParticles(currLoc.Xreal - prevLoc.Xreal,
	//								currLoc.Yreal - prevLoc.Yreal,
	//								currLoc.Yaw  - prevLoc.Yaw);

		this->updatePose();

		this->paintParticles();

		cout << "Desired: " << angleToNext << ", currYaw: " << currLoc.Yaw;
				cout << " i: " << currLoc.Xpos << " j: " << currLoc.Ypos << endl;
	}

	// Go
	while (distance(currLoc.Xpos, currLoc.Ypos, destX , destY) >= normalSpeedRange) {

			hamster->sendSpeed(linearSpeed, 0);

		//	locManager->updateParticles(currLoc.Xreal - prevLoc.Xreal,
		//								currLoc.Yreal - prevLoc.Yreal,
		//								currLoc.Yaw  - prevLoc.Yaw);
			this->updatePose();

			this->paintParticles();

			cout << "DesiredX: " << destX << ", DesiredY: " << destY;
					cout << " i: " << currLoc.Xpos << " j: " << currLoc.Ypos << endl;

		}

/*	while (distance(currLoc.Xpos, currLoc.Ypos, destX , destY) > tolerance){

		hamster->sendSpeed(linearSpeed * slowSpeedRatio, 0);

	//	locManager->updateParticles(currLoc.Xreal - prevLoc.Xreal,
	//								currLoc.Yreal - prevLoc.Yreal,
	//								currLoc.Yaw  - prevLoc.Yaw);

		this->updatePose();

		this->paintParticles();

		cout << "DesiredX: " << destX << ", DesiredY: " << destY;
				cout << " i: " << currLoc.Xpos << " j: " << currLoc.Ypos << endl;


		//FINISH!
		}*/
}

void HamsterUtilities::paintParticles()
{
	vector<Particle *> particlesVector = locManager->getParticles();
	int i = 0;

	for(i = 0; i < particlesVector.size() - TOPFIVE; i++)
	{
		this->paintCell(particlesVector[i]->i, particlesVector[i]->j, RED);
	}

	for(; i < particlesVector.size(); i++)
	{
		this->paintCell(particlesVector[i]->i, particlesVector[i]->j, GREEN);
	}

	this->paintCell(locManager->getMaxBeliefParticle().Xpos, locManager->getMaxBeliefParticle().Ypos, BLUE);

	cv::imshow("blownMap-View",map);

	cv::waitKey(1);

//	map = mapCopy;
	copyEmptyMap();
	sleep(0.5);

	cv::imshow("blownMap-View",map);

	cv::waitKey(1);
}

void HamsterUtilities::copyEmptyMap()
{
	for(int i = 0; i < 1024 ; i++)
		for(int j = 0; j < 1024; j++)
		{
			map.at<cv::Vec3b>(i,j)[0] = mapCopy.at<cv::Vec3b>(i,j)[0];
			map.at<cv::Vec3b>(i,j)[1] = mapCopy.at<cv::Vec3b>(i,j)[1];
			map.at<cv::Vec3b>(i,j)[2] = mapCopy.at<cv::Vec3b>(i,j)[2];
		}
}

HamsterUtilities::~HamsterUtilities() {
	// TODO Auto-generated destructor stub
}

