#include "../headers/Particle.h"
#include <math.h>
#include <iostream>

using namespace std;

Particle::Particle(int x, int y, double yaw)
	: mX(x), mY(y), mYaw(yaw), mBelief(1.0)
{
}

Particle::~Particle()
{
}

double Particle::ProbByMove(int dx, int dy, double dyaw)
{
	// Calculate scores based on yaw and distance
	double distance = sqrt(pow(dx, 2) + pow(dy, 2));
	double distScore = pow(DISTANCE_SCORE_MODIFIER, distance);
	double yawScore = 1 - (abs(dyaw) / 180);

	if (distScore < yawScore)
	{
		return (distScore);
	}
	else
	{
		return (yawScore);
	}
}

double Particle::ProbByMeasures(Robot* robot, Map* map)
{
	int nCorrectReadings = 0;
	int nTotalReadings = 0;
	int nLaserCount = robot->laserCount();
	double fMapResolution = Configuration::Instance()->mapResolution();
	int fMaxLaserDistPx = floor(robot->maxLaserRange() * 100 / fMapResolution);
	int nMapWidth = map->mapWidth();
	int nMapHeight = map->mapHeight();

	// Loop through all the angles
	for (int i = -120; i <= +120; i++)
	{
		// Get the index of the laser reading in the current angle
		int nCurrReadingIdx = this->DegToIndex(i, 240, nLaserCount);

		// Get the reading from the laser
		double fLaserReading = robot->laser(nCurrReadingIdx);

		// Get the actual angle on the map in the direction of the laser
		// TODO: Check if yaw is radian or degree
		double fPointDirection = this->mYaw + i;

		// Get the X and Y modifiers to test the map
		double fModX = +cos(dtor(fPointDirection));
		double fModY = -sin(dtor(fPointDirection));

		/*
		cout << "Reading in angle: " << i << endl
			 << "Reading from index: " << nCurrReadingIdx << endl
			 << "Laser reading: " << fLaserReading << endl
			 << "Laser reading in pixels: " << fLaserReadingPx << endl
			 << "Direction of the reading (deg angle): " << fPointDirection << endl
			 << "X/Y mods: (" << fModX << ", " << fModY << ")" << endl;
		 */

		bool bFoundObstacle = false;
		int nObstacleX;
		int nObstacleY;

		// Loop until we find an obstacle in the direction of the laser
		for (int nParticleDist = 1; nParticleDist < fMaxLaserDistPx && !bFoundObstacle; nParticleDist++)
		{
			// Calculate X and Y for the cell
			int nCurrX = this->mX + nParticleDist * fModX;
			int nCurrY = this->mY + nParticleDist * fModY;

			// Map boundary checks ahead
			// If we hit a map boundary, then we consider it as an obstacle at the boundary point

			// Check X boundaries
			if (nCurrX < 0)
			{
				nCurrX = 0;
				bFoundObstacle = true;
			}
			else if (nCurrX >= nMapWidth)
			{
				nCurrX = nMapWidth - 1;
				bFoundObstacle = true;
			}

			// Check Y boundaries
			if (nCurrY < 0)
			{
				nCurrY = 0;
				bFoundObstacle = true;
			}
			else if (nCurrY >= nMapHeight)
			{
				nCurrY = nMapHeight - 1;
				bFoundObstacle = true;
			}

			// Check if the cell is occupied
			if (map->map()[nCurrY][nCurrX] == OCCUPIED_CELL)
			{
				bFoundObstacle = true;
			}

			// Check if we found an obstacle (either OCCUPIED_CELL hit or boundary hit)
			if (bFoundObstacle)
			{
				nObstacleX = nCurrX;
				nObstacleY = nCurrY;
			}
		}

		double fDistToObstacle;

		// If an obstacle was found, then calculate the distance to it.
		// If an obstacle was not found, use the maximum laser distance
		if (bFoundObstacle)
		{
			fDistToObstacle = sqrt(pow(this->mX - nObstacleX, 2) + pow(this->mY - nObstacleY, 2));
			/*
			cout << "Found obstacle at " << nObstacleX << " , " << nObstacleY << endl;
			cout << "Dist: " << fDistToObstacle << endl
				 << "Dist in meters: " << fDistToObstacle * fMapResolution / 100 << endl;
			 */
		}
		else
		{
			fDistToObstacle = fMaxLaserDistPx;
		}

		// Convert pixels distance to meters
		fDistToObstacle = fDistToObstacle / 100 * fMapResolution;

		// Calculate delta between laser reading and calculated distance from obstacle
		double fDelta = abs(fDistToObstacle - fLaserReading);

		// If the delta is lower than or equal to our defined acceptable noise, we can mark this reading as correct
		if (fDelta <= PARTICLE_LASER_COMPARE_NOISE)
		{
			nCorrectReadings++;
		}

		nTotalReadings++;
	}

	return ((double)nCorrectReadings / nTotalReadings);
}

void Particle::Update(Robot* robot, Map* map)
{
	int dx = robot->dx();
	int dy = robot->dy();
	double dyaw = robot->dyaw();

	this->mX += dx;
	this->mY += dy;
	this->mYaw += dyaw;

	if (this->mYaw < 0)
	{
		this->mYaw += 360;
	}

	double move = this->ProbByMove(dx, dy, dyaw);
	double measures = this->ProbByMeasures(robot, map);
	double last = this->mBelief;
	mes = measures;
	mov = move;
	this->last = last;

	this->mBelief = move * measures * last * NORMALIZATION_FACTOR;

	if (this->mBelief > 1)
	{
		this->mBelief = 1;
	}

#ifdef TEST_MODE
	cout << "P:[" << mX << ", " << mY << ", " << mYaw << "] " <<
			"R:[" << robot->x() << ", " << robot->y() << ", " << robot->yaw() << "]" <<
			"Belief: " << mBelief <<
			"(move: " << move << ")(measures: " << measures << ")(last: " << last << ")" << endl;
#endif
}

Particle* Particle::RandomCloseParticle(Map* map)
{
	int nX = 0;
	int nY = 0;
	double nYaw = this->mYaw + (((rand() % (PARTICLE_CREATE_YAW_RANGE * 2)) - PARTICLE_CREATE_YAW_RANGE) / (double)10);

	// Generate a random particle around the current particle (within PARTICLE_CREATE_IN_RADIUS)
	do
	{
		// Generate (x,y) in range (-PARTICLE_CREATE_IN_RADIUS, +PARTICLE_CREATE_IN_RADIUS)
		nX = this->mX + (rand() % (PARTICLE_CREATE_IN_RADIUS * 2)) - PARTICLE_CREATE_IN_RADIUS;
		nY = this->mY + (rand() % (PARTICLE_CREATE_IN_RADIUS * 2)) - PARTICLE_CREATE_IN_RADIUS;
	}
	while (nX >= 0 && nY >= 0 && nX < map->mapWidth() && nY < map->mapHeight() && map->map()[nY][nX] == OCCUPIED_CELL);

	Particle* random = new Particle(nX, nY, nYaw);
	random->mBelief = this->mBelief;

	return (random);
}
