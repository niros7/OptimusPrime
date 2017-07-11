#ifndef __ROBOT_MANAGER_H__
#define __ROBOT_MANAGER_H__

#include "Robot.h"
#include "Map.h"
#include "Graph.h"
#include "ParticleManager.h"
#include "WaypointManager.h"

class RobotManager
{
	private:
		Graph* mGraph;
		Robot* mRobot;
		Map* mMap;
		ParticleManager* mParticleManager;
		WaypointManager* mWaypointManager;
		int mStartX;
		int mStartY;
		int mGoalX;
		int mGoalY;

	public:
		RobotManager(Robot* robot, Map* map);
		void Start();
};

#endif
