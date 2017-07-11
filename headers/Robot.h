#ifndef __ROBOT_H__
#define __ROBOT_H__

#include <libplayerc++/playerc++.h>
#include <string>
#include "Configuration.h"

using namespace PlayerCc;
using namespace std;

class Robot {
private:
	PlayerClient* mClient;
	Position2dProxy* mPosProxy;
	LaserProxy* mLaserProxy;
	int mLastX;
	int mLastY;
	double mLastYaw;
	int mCurrX;
	int mCurrY;
	double mCurrYaw;
	float* mLasers;
	unsigned int mLaserCount;
	int mMapHeight;
	double mMaxLaserRange;

	// Configuration stuff
	int mWidth;
	int mLength;
	double mMapResolutionMeter;

public:
	Robot(int nMapHeight);
	virtual ~Robot();
	float* GetLaserScan();
	void Read();

	void SetSpeed(float fLinear, float fAngular)
	{
		this->mPosProxy->SetSpeed(fLinear, fAngular);
	}

	int x()
	{
		return (this->mCurrX);
	}

	int y()
	{
		return (this->mCurrY);
	}

	double yaw()
	{
		return (this->mCurrYaw);
	}

	int dx()
	{
		return (this->mCurrX - this->mLastX);
	}

	int dy()
	{
		return (this->mCurrY - this->mLastY);
	}

	double dyaw()
	{
		return (this->mCurrYaw - this->mLastYaw);
	}

	int width()
	{
		return (this->mWidth);
	}

	int length()
	{
		return (this->mLength);
	}

	float laser(int i)
	{
		return (this->mLasers[i]);
	}

	int laserCount()
	{
		return (this->mLaserCount);
	}

	double maxLaserRange()
	{
		return (this->mMaxLaserRange);
	}
};

#endif
