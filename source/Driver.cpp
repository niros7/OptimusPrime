#include "../headers/Driver.h"
#include <iostream>

#define sign(a) ((a) > 0 ? 1 : -1)

Driver::Driver(Robot* robot)
{
	this->mRobot = robot;
	this->mDstX = -1;
	this->mDstY = -1;
	this->mIsDriving = false;
}

void Driver::Drive(Particle* best)
{
	// If we haven't received a destination to drive to, then we shouldn't drive
	if (!this->mIsDriving)
	{
		return;
	}

	double fAngleToWaypoint = this->GetAngleToWaypoint(best);

	// If the robot is facing towards the waypoint, drive straight
	if (this->IsFacingDirection(fAngleToWaypoint, best))
	{
		// TODO: Check speed with a real robot
		this->mRobot->SetSpeed(MOVEMENT_SPEED, 0);
	}
	else
	{
		// Get the angle we need to turn to
		double fCheapestAngle = this->GetCheapestAngleToTurn(fAngleToWaypoint, best);

		// If the angle is positive, we need to turn left
		// If the angle is negative, we need to turn right
		// Get the multiplier which tells us which way to turn (multiply by robot's angular speed)
		int nTurnMultiplier = sign(fCheapestAngle);

		// TODO: Check speed with a real robot
		this->mRobot->SetSpeed(0, TURNING_SPEED * nTurnMultiplier);
	}
}

double Driver::GetCheapestAngleToTurn(double fTowardsAngle, Particle* best)
{
	// Get the angles
	double fRobotAngle = best->yaw();

	// Calculate turn cost for left turn and right turn
	double fLeft = fTowardsAngle - fRobotAngle;
	double fRight = fRobotAngle - fTowardsAngle;

	// Fix angles

	if (fLeft < 0)
	{
		fLeft += 360;
	}

	if (fRight < 0)
	{
		fRight += 360;
	}

	// Check which direction is better
	if (fLeft < fRight)
	{
		return (+fLeft);
	}
	else
	{
		return (-fRight);
	}
}

bool Driver::IsFacingDirection(double fAngle, Particle* best)
{
	// Check if robot's yaw is equal to (around) the calculated angle
	if (abs(fAngle - best->yaw()) <= ANGLE_COMPARISON_ACCEPTABLE_NOISE)
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

double Driver::GetAngleToWaypoint(Particle* best)
{
	// Calculate delta from waypoint to robot
	int dy = best->y() - this->mDstY;
	int dx = this->mDstX - best->x();

	// Calculate the angle between robot to waypoint
	double fAngleToWaypoint = rtod(atan2(dy, dx));

	if (fAngleToWaypoint < 0)
	{
		fAngleToWaypoint += 360;
	}

	return (fAngleToWaypoint);
}
