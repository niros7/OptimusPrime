#include <iostream>
#include "../headers/RobotManager.h"

using namespace std;

RobotManager::RobotManager(Robot* robot, Map* map)
{
	this->mMap = map;
	this->mRobot = robot;
	this->mGraph = new Graph(map);
	this->mParticleManager = new ParticleManager(map);
	this->mWaypointManager = new WaypointManager(robot, map, this->mGraph);
	this->mStartX = Configuration::Instance()->start().x;
	this->mStartY = Configuration::Instance()->start().y;
	this->mGoalX = Configuration::Instance()->goal().x;
	this->mGoalY = Configuration::Instance()->goal().y;
}

void RobotManager::Start()
{
	this->mWaypointManager->SetDestination(this->mStartX, this->mStartY, this->mGoalX, this->mGoalY);

	// Start the execution of the robot
	while (!this->mWaypointManager->ReachedDestination())
	{
		// Read values from the robot
		this->mRobot->Read();

		// Update the particle manager and get the best particle

#ifdef USE_PARTICLES
		Particle* best = this->mParticleManager->Update(this->mRobot, this->mMap);
#else
		Particle* best = new Particle(this->mRobot->x(), this->mRobot->y(), this->mRobot->yaw());
#endif

		// Update the waypoint manager
		this->mWaypointManager->Update(best);
	}
}
