#include "LocalizationManager.h"
#include "Location.h"
#include <ctime>
using namespace std;

LocalizationManager::LocalizationManager(OccupancyGrid blownMap, HamsterAPI::Hamster *hamster, float resolution) : blownMap(blownMap), hamster(hamster), resolution(resolution){
	cout << "width: " << this->blownMap.getWidth() << ", height: " << this->blownMap.getHeight() << endl;
	this->width = this->blownMap.getWidth();
	this->height = this->blownMap.getHeight();

	initParticles();
}

//initialize the particle vector and gives each a random location
void LocalizationManager::initParticles() {
	srand ( time(NULL) );

	this->particlesVector.resize(PARTICLES_NUM);

	for (int i = 0; i < this->particlesVector.size(); i++) {
		this->particlesVector[i] = new Particle();
		getRandomLocation(this->particlesVector[i]);
	}
}

void LocalizationManager::addGridIndexFromMapLoc(Particle *particle)
{
	particle->i = ((double)(this->height / 2)) - (particle->y / this->resolution) ;
	particle->j = (particle->x / this->resolution) + ((double)(this->width / 2));
}

void LocalizationManager::addMapLocFromGridIndex(Particle *particle)
{
	particle->x = (particle->j - (double)(this->width / 2)) * this->resolution;
	particle->y = (((double)(this->height / 2)) - particle->i) * this->resolution;
}


//Gives  a random location (i and j) for each particle
void LocalizationManager::getRandomLocation(Particle *particle) {
	srand ( time(NULL) );

	do {
		particle->j = rand() % this->width;
		particle->i = rand() % this->height;
	} while (!this->isCellFree(particle->i, particle->j));

	addMapLocFromGridIndex(particle);

	particle->yaw = rand() % 360;
}

double LocalizationManager::computeBelief(Particle *particle) {
	LidarScan scan = hamster->getLidarScan();

	int hits = 0;
	int misses = 0;

	for (int i = 0; i < scan.getScanSize(); i++) {
		double angle = scan.getScanAngleIncrement() * i * DEG2RAD;// the zavit of the robot

		//ignore the distances that out of the laser max range
		if (scan.getDistance(i) < scan.getMaxRange() - 0.001) {
			// calculate the obstacle pos for each degree

			double obsX = particle->x + scan.getDistance(i) * cos(angle + 90*DEG2RAD + particle->yaw * DEG2RAD);
			double obsY = particle->y + scan.getDistance(i) * sin(angle + 90*DEG2RAD + particle->yaw * DEG2RAD);

			int pixelsY = (double)(this->height / 2) - obsY / this->resolution;
			int pixelsX = obsX / this->resolution + (double)(this->width / 2);

			if (!this->isCellFree(pixelsY, pixelsX)) {
				hits++;
			} else {
				misses++;
			}
		}
	}
	return (float)hits / (hits + misses);
}

void LocalizationManager::updateParticles(double deltaX, double deltaY, double deltaYaw) {
	for (int i = 0; i < this->particlesVector.size(); i++) {

		Particle *particle = this->particlesVector[i];

		double r = sqrt(deltaX * deltaX + deltaY * deltaY);
				particle->x += r * cos(particle->yaw * DEG2RAD);
				particle->y += r * sin(particle->yaw * DEG2RAD);

		particle->yaw = fmod(particle->yaw + deltaYaw, 360);

		addGridIndexFromMapLoc(particle);

		int x = particle->x;
		int y = particle->y;
		int i2 = particle->i;
		int j2 = particle->j;

		// If the updated particle is out of bounds
		if (!this->isCellFree(particle->i, particle->j))
		{
			bool isSuccessfullyInserted = false;

			if (particle->belief > 0.3)
			{
				// Try to get the particle back in
				isSuccessfullyInserted = insertOutOfBoundsParticle(particle);
			}

			if (!isSuccessfullyInserted)
			{
				Particle * betterParticle =
						particlesVector.at(particlesVector.size() - ( rand() % BEST_PARTICLE ) - 1);

				if (betterParticle->belief > 0.4)
				{	// The particle had a low belief
					createChildFromFather(particle, betterParticle);
				}
				else
				{	// No high belief particles exist so we randomize in any free slot
					getRandomLocation(particle);
				}
			}
		}

		particle->belief = computeBelief(particle);
	}

	this->getChildren();
}

bool LocalizationManager::insertOutOfBoundsParticle(Particle *particle)
{
	Particle * copyParticle = new Particle(*particle);
		int dist, count = 0;

		do
		{
			// Try 20 times to to get the particle back in to free spot next
			// to the point it was before it went out
			dist = 10 - rand() % 20;
			particle->j = copyParticle->j + dist;

			dist = 10 - rand() % 20;
			particle->i = copyParticle->i + dist;

			count++;
		} while (!this->isCellFree(particle->i, particle->j) && count < 20);

		// Convert indices to location on map
		addMapLocFromGridIndex(particle);

		delete copyParticle;

		return count < 20;
}

void LocalizationManager::getChildren()
{
	srand ( time(NULL) );
	std::sort(this->particlesVector.begin(),this->particlesVector.end(),lessThanBelief);

	for(int i = 0; i < DEL_PARTICLE; i++)
	{
		int angle, rand_distance;
		int randFather = 0;
		int counter = 0;

		Particle *childParticle = new Particle();
		Particle *fatherParticle;

		randFather = rand()%(PARTICLES_NUM - DEL_PARTICLE) + DEL_PARTICLE;
		fatherParticle = this->particlesVector[randFather];

		if (fatherParticle->belief > 0.3)
		{
			createChildFromFather(childParticle, fatherParticle);
		}
		else
		{
			getRandomLocation(childParticle);
		}

		childParticle->belief = computeBelief(childParticle);
		this->particlesVector[i] = childParticle;
	}
}

void LocalizationManager::createChildFromFather(Particle *childPart, Particle *fatherPart)
{
	int counter = 0;

	do
		{
			counter++;

			if (fatherPart->belief < 0.3)
			{
				childPart->x = fatherPart->x + 0.4 - 0.8*(double)rand()/(double)RAND_MAX;
				childPart->y = fatherPart->y + 0.4 - 0.8*(double)rand()/(double)RAND_MAX;
			}
			else if (fatherPart->belief < 0.6)
			{
				childPart->x = fatherPart->x + 0.2 - 0.4*(double)rand()/(double)RAND_MAX;
				childPart->y = fatherPart->y + 0.2 - 0.4*(double)rand()/(double)RAND_MAX;
			}
			else
			{
				childPart->x = fatherPart->x + 0.1 - 0.2*(double)rand()/(double)RAND_MAX;
				childPart->y = fatherPart->y + 0.1 - 0.2*(double)rand()/(double)RAND_MAX;
			}

			addGridIndexFromMapLoc(childPart);

			if (counter > 100)
			{
				getRandomLocation(childPart);
			}

		} while (!this->isCellFree(childPart->i, childPart->j));

	// Randomizing the angle according to the belief of the goodParticle
		if (fatherPart->belief < 0.2)
		{
			childPart->yaw = (fatherPart->yaw + (180 - rand() % 360));
		}
		else if (fatherPart->belief < 0.4)
		{
			childPart->yaw = (fatherPart->yaw + (90 - rand() % 180));
		}
		else if (fatherPart->belief < 0.6)
		{
			childPart->yaw = (fatherPart->yaw + (30 - rand() % 60));
		}
		else if (fatherPart->belief < 0.8)
		{
			childPart->yaw = (fatherPart->yaw + (10 - rand() % 20));
		}
		else
		{
			childPart->yaw = (fatherPart->yaw + (5 - rand() % 10));
		}

		//Reducing degrees - modulo 360
		childPart->yaw = (childPart->yaw >= 360) ?
				childPart->yaw - 360 : childPart->yaw;
		childPart->yaw = (childPart->yaw < 0) ?
				childPart->yaw + 360 : childPart->yaw;
}

void LocalizationManager::getParticleIfCellNotFree(Particle *particle) {
	do{

		int angle = rand() % 360;
		int radius = rand() % RADIUS;
		particle->j = (particle->j + radius * cos (angle));
		particle->i = (particle->i + radius * sin (angle));

	} while (!this->isCellFree(particle->i, particle->j));
	particle->yaw = rand() % 360;
	particle-> x = (particle->j - (double)(this->width / 2)) * this->resolution;
	particle-> y = ((double)(this->height / 2) - particle->i) * this->resolution;
	particle->belief = computeBelief(particle);

}

bool LocalizationManager::isCellFree(int x, int y)
{
	bool isCellFree = false;

	if(this->blownMap.getCell(x,y) == CELL_FREE)
	{
		if ( x > 0 && y < 1024 &&
			 x > 0 && y < 1024)
			isCellFree = true;
	}

	return isCellFree;
}

Location LocalizationManager::getMaxBeliefParticle()
{
	float maxBel = 0;
	int maxIndex = 0;
	int index = 0;

	for (;  index < particlesVector.size() ; index++)
	{
		if (particlesVector[index]->belief > maxBel)
		{
			maxBel = particlesVector[index]->belief;
			maxIndex = index;
		}
	}

	Particle *maxP = this->getParticles()[maxIndex];

	Location returnLocation;
	returnLocation.Xpos = maxP->i;
	returnLocation.Ypos = maxP->j;
	returnLocation.Xreal = maxP->x;
	returnLocation.Yreal = maxP->y;
	returnLocation.Yaw = maxP->yaw;

	return returnLocation;
}

vector<Particle *> LocalizationManager::getTopFiveParticles()
{
	std::sort(this->particlesVector.begin(),this->particlesVector.end(),lessThanBelief);

	vector<Particle *> topFiveVector;
	topFiveVector.resize(TOPFIVE);
	int j = 0;

	for(int i = this->particlesVector.size() - 1; i > (this->particlesVector.size() - 1 - TOPFIVE); i--)
	{
		topFiveVector[j] = this->particlesVector[i];
		j++;
	}

	return topFiveVector;
}

void LocalizationManager::printParticles() const {
	for (int i = 0; i < particlesVector.size(); i++) {
		Particle *particle = particlesVector[i];
		cout << "Particle " << i << ": " << particle->x << "," << particle->y << ", belief: " << particle->belief << endl;
	}
}

vector<Particle *> LocalizationManager::getParticles() const {
	return particlesVector;
}

LocalizationManager::~LocalizationManager() {
}
