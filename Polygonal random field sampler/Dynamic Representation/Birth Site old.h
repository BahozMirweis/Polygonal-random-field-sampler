#pragma once
#include "Particle.h"

class BirthSiteOld {
private:
	double displacement, time;
	double velocity;
public:
	bool boundarySite;
	bool created;
	std::vector<Particle> particles;

	BirthSiteOld(double displacement, double time, bool boundarySite, double velocity = 0.0);
	double particleBirthDensity(double u, double v) {
		return std::log(std::abs(u - v)) - 1.5 * std::log(1 + u * u) - 1.5 * std::log(1 + v * v);
	}

	double getTime();
	double getDisplacement();
};