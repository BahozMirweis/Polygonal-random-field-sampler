#include "Particle.h"
#include <cmath>
#include <armadillo>
#include "Metropolis Hastings.h"
#include <functional>

Particle::Particle(double CreationTime, double initVelocity, double initialDisplacement) : velocity{ initVelocity }, displacement{ initialDisplacement }, initDisplacement{ initialDisplacement }, err { 0.1 },
time{ CreationTime }, creationTime{CreationTime},  jumpPoints { {time, displacement}},
sample{ true }, control{ true } {
	std::function<double(double)> timeDensity{ [this](double x) {return this->jumpTimeLogDensity(x); } };

	jumpTime = time + MetropolisHastings::sample1D(timeDensity, 0.0, 0.5, 50000);
}

Particle::Particle() : Particle(0.0, 0.0, 0.0) {};

// Particle parameter is below particle
bool Particle::collision(const Particle& p) const {
	return displacement < p.displacement;
}

bool Particle::collisionDeath(const Particle& p) const {
	return std::abs(jumpPoints.back().first - p.jumpPoints.back().first) < err && std::abs(jumpPoints.back().second - p.jumpPoints.back().second) < err;
}

bool Particle::finalCollision(const Particle& p) const {
	return false;
}

void Particle::moveParticle(double time) {
	double timeChange = time - this->time;
	this->time = time;

	displacement += timeChange * velocity;
}

double Particle::jumpLogDensity(double u) const {  
	return std::log(std::abs(u - velocity)) - 1.5 * std::log(1 + u * u);
}

double Particle::jumpTimeLogDensity(double x) const {
	if (x < 0) {
		return -2147483648;
	}

	return -2 * std::sqrt(1 + velocity*velocity) * x;
}

void Particle::jumpNewVelocity() {
	std::function<double(double)> density{ [this](double u) {return this->jumpLogDensity(u); } };
	std::function<double(double)> timeDensity{ [this](double x) {return this->jumpTimeLogDensity(x); } };

	velocity = MetropolisHastings::sample1D(density, 0.0, 0.25, 50000);

	jumpTime = time + MetropolisHastings::sample1D(timeDensity, 0.0, 0.5, 50000);
}

bool Particle::updateParticle(double time, double maxBoundary, double minBoundary) {
	
	// Jumps to new velocity
	if (time >= jumpTime) {

		if (sample) {
			jumpPoints.push_back({ time, displacement });
			jumpNewVelocity();
		}
		else {
			if (jumpPosition + 1 == jumpPoints.end()) return false;

			jumpTime = (*(jumpPosition + 1)).first;
			velocity = ((*jumpPosition).second - (*(jumpPosition + 1)).second) / ((*jumpPosition).first - (*(jumpPosition + 1)).first);

			if (control) {
				jumpPosition = jumpPoints.erase(jumpPosition);
			}
			else {
				++jumpPosition;
			}


		}

		prevVel = velocity;
	}
	else {
		prevVel = 0;
	}

	if (time < this->time) {
		return true;
	}

	moveParticle(time);

	if (displacement > maxBoundary || displacement < minBoundary) {
		return false;
	}

	

	return true;
}

double Particle::getDisplacement() const {
	return displacement;
}

void Particle::endMotion(Particle* killedBy) {
	if (sample) {
		if (killedBy) {
			if (prevVel != 0) {
				velocity = prevVel;
				prevVel = 0.0;
			}

			if (killedBy->prevVel != 0) {
				killedBy->velocity = prevVel;
				prevVel = 0.0;
			}

			if (killedBy->displacement == killedBy->initDisplacement) {
				jumpPoints.push_back(killedBy->jumpPoints.back());
			}
			else {
				double c1 = displacement - velocity * time;

				// use value from jump points vector
				double c2 = killedBy->displacement - killedBy->velocity * killedBy->time;
				double intTime = (c1 - c2) / (killedBy->velocity - velocity);
				double intDisplacement = velocity * intTime + c1;
				jumpPoints.push_back({ intTime, intDisplacement });
			}
		}
		else {
			jumpPoints.push_back({ this->time, this->displacement });
		}
	} 

	if (!sample && control) {
		while (jumpPosition != jumpPoints.end()) {
			jumpPosition = jumpPoints.erase(jumpPosition);
		}
	}

	sample = false;
	control = false;
	this->displacement = initDisplacement;
	this->time = creationTime;

	if (jumpPoints.size() != 0) {
		jumpTime = jumpPoints[0].first;
		jumpPosition = jumpPoints.begin();
		velocity = ((*(jumpPosition + 1)).second - (*jumpPosition).second) / ((*(jumpPosition + 1)).first - (*jumpPosition).first);
	}
}

void Particle::changeParticle(Particle* killedBy) {
	if (!sample && control) {
		if (killedBy) {
			if (prevVel != 0) {
				velocity = prevVel;
				prevVel = 0.0;
			}

			if (killedBy->prevVel != 0) {
				killedBy->velocity = prevVel;
				prevVel = 0.0;
			}

			if (killedBy->displacement == killedBy->initDisplacement) {
				jumpPosition = jumpPoints.insert(jumpPosition, killedBy->jumpPoints.back()) + 1;
			}
			else {
				double c1 = displacement - velocity * time;
				double c2 = killedBy->displacement - killedBy->velocity * killedBy->time;
				double intTime = (c1 - c2) / (killedBy->velocity - velocity);
				double intDisplacement = velocity * intTime + c1;
				jumpPosition = jumpPoints.insert(jumpPosition, { intTime, intDisplacement }) + 1;
			}
		}
		else {
			jumpPosition = jumpPoints.insert(jumpPosition, { time, displacement }) + 1;
		}
	}
	else if (control) {
		jumpPoints.pop_back();
	}
}

double Particle::getVelocity() const {
	return velocity;
}