#include "../headers/Robot.h"

Robot::Robot(int nMapHeight)
{
	// Read configuration and initialize variables
	Configuration* config = Configuration::Instance();
	this->mWidth = config->robotSize().width;
	this->mLength = config->robotSize().length;
	this->mLastX = this->mCurrX = config->start().x;
	this->mLastY = this->mCurrY = config->start().y;
	this->mLastYaw = this->mCurrYaw = config->start().yaw;
	this->mMapResolutionMeter = config->mapResolution() / 100;
	this->mClient = new PlayerClient("localhost", 6665);
	this->mLaserProxy = new LaserProxy(this->mClient);
	this->mPosProxy = new Position2dProxy(this->mClient);
	this->mLaserCount = 0;
	this->mMaxLaserRange = 4;
	this->mLasers = NULL;
	this->mMapHeight = nMapHeight;

	// Enable motors
	this->mPosProxy->SetMotorEnable(true);

	// Set the start position of the robot
	this->mPosProxy->SetOdometry(
			this->mCurrX * this->mMapResolutionMeter,
			(this->mMapHeight - this->mCurrY) * this->mMapResolutionMeter,
			dtor(this->mCurrYaw));

	// Read values
	this->Read();
}

Robot::~Robot()
{
}

void Robot::Read()
{
	// Read stuff from robot proxy
	this->mClient->Read();

	// Set last pos as old current pos
	this->mLastX = this->mCurrX;
	this->mLastY = this->mCurrY;
	this->mLastYaw = this->mCurrYaw;

	// Set current pos with new pos
	this->mCurrX = this->mPosProxy->GetXPos() / this->mMapResolutionMeter;
	this->mCurrY = (this->mMapHeight) - (this->mPosProxy->GetYPos() / this->mMapResolutionMeter);
	this->mCurrYaw = rtod(this->mPosProxy->GetYaw());

	// Yaw fix
	if (this->mCurrYaw < 0)
	{
		this->mCurrYaw += 360;
	}

	this->mLaserCount = this->mLaserProxy->GetCount();

	// Reset lasers array
	if (this->mLasers != NULL)
	{
		delete[] this->mLasers;
		this->mLasers = NULL;
	}

	// Rebuild lasers array
	this->mLasers = new float[this->mLaserCount];
	this->mMaxLaserRange = this->mLaserProxy->GetMaxRange();

	for (unsigned i = 0; i < this->mLaserCount; i++)
	{
		this->mLasers[i] = (*this->mLaserProxy)[i];
	}
}
