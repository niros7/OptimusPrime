#ifndef LOCALIZATIONMANAGER_H_
#define LOCALIZATIONMANAGER_H_

#include "Particle.h"
#include "Location.h"
#include <vector>
#include <HamsterAPIClientCPP/Hamster.h>
#include <iostream>
#include <cmath>

#define PARTICLES_NUM 350
#define DEL_PARTICLE 80
#define BEST_PARTICLE 80
#define RADIUS 8
#define ROULETTE 5
#define TRASHOLD 0.5
#define TOPFIVE 5

using namespace std;
using namespace HamsterAPI;

class LocalizationManager {
private:

	vector<Particle *> particlesVector;
	OccupancyGrid blownMap;//Blown map
	float resolution;
	int width;
	int height;
	Hamster *hamster;
	void getRandomLocation(Particle *particle);
	double computeBelief(Particle *particle);

	struct compClass //comparator for the sort
	{
		bool operator()(Particle * par1 , Particle * par2){
			return (par1->belief < par2->belief);
		}
	}lessThanBelief;

public:
	void getChildren();
	void getParticleIfCellNotFree(Particle *particle);
	void getChildParticles();
	LocalizationManager(OccupancyGrid blownMap, HamsterAPI::Hamster *hamster, float resolution);
	void initParticles();
	void updateParticles(double deltaX, double deltaY, double deltaYaw);
	bool insertOutOfBoundsParticle(Particle *particle);
	void createChildFromFather(Particle *childPart, Particle *fatherPart);
	void addGridIndexFromMapLoc(Particle *particle);
	void addMapLocFromGridIndex(Particle *particle);
	bool isCellFree(int x, int y);
	struct Location getMaxBeliefParticle();
	vector<Particle *> getTopFiveParticles();

	void printParticles() const;
	vector<Particle *> getParticles() const;

	virtual ~LocalizationManager();
};

#endif /* LOCALIZATIONMANAGER_H_ */
