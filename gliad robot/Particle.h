#ifndef PARTICLE_H_
#define PARTICLE_H_

class Particle {
public:
	/// index in the map
	unsigned int i, j;

	//the real index in the real location
	double x, y;
	double yaw;
	double belief;
};

#endif /* PARTICLE_H_ */
