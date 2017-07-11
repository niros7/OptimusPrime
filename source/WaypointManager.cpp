#include "../headers/WaypointManager.h"
#include <iostream>
using namespace std;

WaypointManager::WaypointManager(Robot* robot, Map* map, Graph* graph)
{
	this->mRobot = robot;
	this->mMap = map;
	this->mGraph = graph;
	this->mCurrentTarget = NULL;
	this->mDriver = new Driver(robot);
	this->mReachedDestination = false;
}

void WaypointManager::SetDestination(int nStartX, int nStartY, int nGoalX, int nGoalY)
{
	// Clear the stack
	while (!this->mPaths.empty())
	{
		this->mPaths.pop();
	}

	// Halt robot while we're computing stuff
	this->mRobot->SetSpeed(0, 0);

	// Perform A*
	vector<Node*> path = this->mGraph->CalculatePath(nStartX, nStartY, nGoalX, nGoalY);

	this->mPaths.push(NULL);

	// Move the elements into the stack
	for (unsigned i = 0; i < path.size(); i++)
	{
		if (path[i] != NULL)
		{
			this->mPaths.push(path[i]);
		}
	}

	this->NextTarget();
}

void WaypointManager::Update(Particle* best)
{
	if (this->mCurrentTarget != NULL)
	{
		// Translate particle position to grid
		double fMapToGrid = Configuration::Instance()->gridResolution() / Configuration::Instance()->mapResolution();
		int nX = best->x() / fMapToGrid;
		int nY = best->y() / fMapToGrid;

		// Calculate deltas
		int dx = abs(this->mCurrentTarget->x() - nX);
		int dy = abs(this->mCurrentTarget->y() - nY);

		int nAllowedRadius = ROBOT_REACHED_WAYPOINT_RADIUS;

		// If the current waypoint is the last waypoint, the robot must go to it exactly
		if (this->mPaths.size() == 0 || this->mPaths.top() == NULL)
		{
			nAllowedRadius = 0;
		}

		// Check if robot reached target cell in grid
		if (dx <= nAllowedRadius && dy <= nAllowedRadius)
		{
			this->NextTarget();
		}
	}

	this->mDriver->Drive(best);
}

void WaypointManager::NextTarget()
{
	Node* next = NULL;

	// If there are places to be, we shall go to the next place
	if (!this->mPaths.empty())
	{
		next = this->mPaths.top();
		this->mPaths.pop();
	}

	// If there are no places to be (or we've reached the end)
	if (next == NULL)
	{
		this->mRobot->SetSpeed(0, 0);
		this->mDriver->StopDriving();
		cout << "[WAZE] You have reached your destination" << endl;
		this->mReachedDestination = true;
	}
	else
	{
		double fResolution = Configuration::Instance()->gridResolution() / Configuration::Instance()->mapResolution();
		int x = next->x() * fResolution;
		int y = next->y() * fResolution;
		this->mDriver->SetDestination(x, y);
		cout << "[WAZE] in 200 meters, go to (" << x << "," << y << ")" << endl;
	}

	this->mCurrentTarget = next;
}
