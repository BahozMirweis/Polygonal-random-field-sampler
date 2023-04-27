#pragma once
#include <vector>
#include <armadillo>
#include <memory>

// An issue to be careful about is that the velocity of a particle could be arbitrarily large (e.g. 10^6) which could cause major issues as the particles only store their displacement
// This opens up issues when we compare displacements using the error because their displacements would be greater than the error.
// One simple way round this is to store the final displacement equally when both particles intersect then we compare this equally. However I have big laziness.

class Particle {
private:
	double velocity;
	double displacement;
	double initDisplacement;
	double creationTime;
	double time;
	double jumpTime;
	double err;
public:
	bool sample;
	bool control;
	double prevVel{0};

	std::vector<std::pair<double, double>> jumpPoints;

	// for when we repeat the particle motion
	std::vector<std::pair<double, double>>::iterator jumpPosition;
	
	Particle();

	Particle(const Particle&) = default;

	Particle& operator=(const Particle&) = default;

	Particle(double CreationTime, double initVelocity, double initialDisplacement);

	bool collision(const Particle& p) const;

	bool collisionDeath(const Particle& p) const;

	// when we want to check if two particles die at the same point
	bool finalCollision(const Particle& p) const;

	void moveParticle(double time);

	bool updateParticle(double time, double maxBoundary, double minBoundary);

	double jumpLogDensity(double u) const;

	double jumpTimeLogDensity(double x) const;

	void jumpNewVelocity();

	double getDisplacement() const;

	double getVelocity() const;

	void endMotion(Particle* killedBy = nullptr);

	// whenn we want to destroy remaining edges.
	void changeParticle(Particle* killedBy = nullptr);
};