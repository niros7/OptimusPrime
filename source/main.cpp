#include "../headers/Map.h"
#include "../headers/Configuration.h"
#include "../headers/Robot.h"
#include "../headers/RobotManager.h"
#include "../headers/Particle.h"
#include <iostream>

#define CONFIG_PATH "/usr/robotics/PcBotWorldNew/params/parameters.txt"

using namespace std;

//int main()
//{
//	Configuration::Init(CONFIG_PATH);
//	Map* map = new Map();
//	Robot* robot = new Robot(map->mapHeight());

//	RobotManager* manager = new RobotManager(robot, map);

//	manager->Start();
//}
