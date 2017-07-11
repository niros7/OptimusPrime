#ifndef __WAYPOINT_MANAGER_H__
#define __WAYPOINT_MANAGER_H__

#define ROBOT_REACHED_WAYPOINT_RADIUS 2

#include <stack>
#include "Map.h"
#include "Graph.h"
#include "Node.h"
#include "Robot.h"
#include "Driver.h"
#include "Configuration.h"

class WaypointManager
{
	private:
		stack<Node*> mPaths;
		Robot* mRobot;
		Map* mMap;
		Graph* mGraph;
		Node* mCurrentTarget;
		Driver* mDriver;
		bool mReachedDestination;

		void NextTarget();

	public:
		WaypointManager(Robot* robot, Map* map, Graph* graph);

		void SetDestination(int nStartX, int nStartY, int nGoalX, int nGoalY);
		void Update(Particle* best);

		bool ReachedDestination()
		{
			return (this->mReachedDestination);
		}
};

#endif
