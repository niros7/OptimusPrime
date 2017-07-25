#include <HamsterAPIClientCPP/Hamster.h>
#include <iostream>
#include "HamsterUtilities.h"
#include "LocalizationManager.h"
#include <vector>
#include <string>
#include <iomanip>
#include "Astar.h"
#include "ConfigFile.h"

using namespace std;
using namespace HamsterAPI;


const int dx2[dir]={1, 1, 0, -1, -1, -1, 0, 1};
const int dy2[dir]={0, 1, 1, 1, 0, -1, -1, -1};

int main() {
	try {

		Hamster* hamster = new HamsterAPI::Hamster(1);

		cv::namedWindow("blownMap-View");
		cv::namedWindow("originalMap-View");
		while (hamster->isConnected()) {
			try {

				ConfigFile *conf;
				conf = ConfigFile::GetInstance();

				conf->ReadConfigFile();


				OccupancyGrid ogrid = hamster->getSLAMMap();
				int width = ogrid.getWidth();
				int height = ogrid.getHeight();
				int mapForPath[height * width];

				OccupancyGrid navMap = hamster->getSLAMMap();

				//CvMat* M = cvCreateMat(width, height, CV_8UC1);
				cv::Mat blownMap = cv::Mat(width, height,CV_8UC3);
				cv::Mat originalMap = cv::Mat(width, height,CV_8UC3);

				LocalizationManager* localizationManager =
						new LocalizationManager(navMap, hamster, navMap.getResolution());;

				// Initialize utilities class
				HamsterUtilities* hamsterUtilities =
						new HamsterUtilities(hamster, conf->hamsterRadius, blownMap,
											 ogrid.getResolution(), localizationManager, navMap);

				hamsterUtilities->blowMap(ogrid);

				// Setting the map for Astar
				for (int i = 0; i < height; i++)
					for (int j = 0; j < width; j++) {
						if (blownMap.at<cv::Vec3b>(i,j)[0] == 255)
							mapForPath[i * width + j] = 0;
						else if (blownMap.at<cv::Vec3b>(i,j)[0] == 0 ||
								 blownMap.at<cv::Vec3b>(i,j)[0] == 169)
							mapForPath[i * width + j] = 1;
					}

				string route =
						pathFind(conf->startX, conf->startY,
								 conf->goalX, conf->goalY, mapForPath,height, width);

				int nWayPoints = hamsterUtilities->paintRoad(conf->startX,conf->startY, route);

				for (int index = 0; index < nWayPoints; index++)
				{
					hamsterUtilities->MoveToNextWayPoint(index);
				}

				cv::imshow("blownMap-View",blownMap);

			//	cv::imshow("originalMap-View",originalMap);
				cv::waitKey(1);

			} catch (const HamsterAPI::HamsterError & message_error) {
				HamsterAPI::Log::i("Client", message_error.what());
			}

		}
	} catch (const HamsterAPI::HamsterError & connection_error) {
		HamsterAPI::Log::i("Client", connection_error.what());
	}
	return 0;
}
