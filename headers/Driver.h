#ifndef __DRIVER_H__
#define __DRIVER_H__

#include "Robot.h"
#include "Particle.h"

#define ANGLE_COMPARISON_ACCEPTABLE_NOISE 	5	// must turn if not in this range
#define MOVEMENT_SPEED 				 		2	// default movement speed (when walking straight)
#define TURNING_SPEED  				 		0.5	// turning speed

class Driver
{
	private:
		int mDstX;
		int mDstY;
		bool mIsDriving;
		Robot* mRobot;

		double GetCheapestAngleToTurn(double fTowardsAngle, Particle* best);
		bool IsFacingDirection(double fAngle, Particle* best);
		double GetAngleToWaypoint(Particle* best);
	public:
		Driver(Robot* robot);
		void Drive(Particle* best);

		void SetDestination(int mDstX, int mDstY)
		{
			this->mDstX = mDstX;
			this->mDstY = mDstY;
			this->mIsDriving = true;
		}

		void StopDriving()
		{
			this->mIsDriving = false;
		}
};

#endif
