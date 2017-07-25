/*
 * HamsterUtilities.h
 *
 *  Created on: May 19, 2017
 *      Author: user
 */

#ifndef HAMSTERUTILITIES_H_
#define HAMSTERUTILITIES_H_

#include <HamsterAPIClientCPP/Hamster.h>
#include <string>
#include <vector>
#include <iomanip>
#include "LocalizationManager.h"
#include "Location.h"


const double tolerance = 1;
const double normalSpeedRange = 10 * tolerance;
const double linearSpeed = 0.27;
const double angleTolerange = 10;
const float angularSpeed = 0.5;
const int turningAngle = 45;
const double slowSpeedRatio = 0.1;
const int topFive = 5;

class HamsterUtilities {
	private:
		cv::Mat map;
		cv::Mat mapCopy;
		OccupancyGrid navMap;
		int height;
		int width;
		double resolution;
		double hamsterRadius;
		Location* wayPoints;
		HamsterAPI::Hamster* hamster;
		LocalizationManager* locManager;
		Location currLoc;
		Location prevLoc;

	public:
		HamsterUtilities(HamsterAPI::Hamster* hamster , double hamsterRadius,
				        cv::Mat map, double resolution, LocalizationManager* locManager,
						OccupancyGrid navMap);

		void blowMap(OccupancyGrid ogrid);
		void blowSurroundings(int x, int y, int pixelColor);
		double distance(double x1, double y1, double x2, double y2);
		void paintCell(int x, int y, int color);
		void updatePose();
		Location getCurrPose();
		int paintRoad(int StartX, int StartY, std::string route);
		void MoveToNextWayPoint(int index);
		void copyEmptyMap();
		void paintParticles();
		virtual ~HamsterUtilities();

		static const int BLACK = 1;
		static const int WHITE = 2;
		static const int GREEN = 3;
		static const int YELLOW = 4;
		static const int BLUE = 5;
		static const int RED = 6;
		static const int GRAY = 7;
};

#endif /* HAMSTERUTILITIES_H_ */
