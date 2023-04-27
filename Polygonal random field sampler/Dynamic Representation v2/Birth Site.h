#pragma once

#pragma once
#include <vector>
#include <math.h>

class BirthSite {
private:
	double displacement, time;
public:
	bool boundarySite;
	std::vector<double> velocities;

	BirthSite(double displacement, double time, bool boundarySite, double velocity = 0.0);
	double particleBirthDensity(double u, double v) {
		return log(std::abs(u - v)) - 1.5 * log(1 + u * u) - 1.5 * log(1 + v * v);
	}

	double getTime() const;
	double getDisplacement() const;
};