#include "Birth Site old.h"
#include <functional>
#include <armadillo>
#include "Metropolis Hastings.h"

//velocity should only be defined at boundary site.
BirthSiteOld::BirthSiteOld(double displacement, double time, bool boundarySite, double velocity) : displacement{ displacement }, time{ time }, boundarySite{ boundarySite }, velocity{ velocity }, created{false} {
	if (!boundarySite) {
		std::function<double(arma::vec)> particleDensity{ [this](arma::vec velocities) {return this->particleBirthDensity(velocities.at(0), velocities.at(1)); } };

		arma::vec sampledVelocities{ MetropolisHastings::sample2D(particleDensity, arma::vec{ {0.0, 0.0} }, arma::mat{ {1, 0.5}, {0.5, 1} }, 50000) };

		// to make dynamic graph adding particles easier.
		particles.push_back(Particle(time, std::min(sampledVelocities.at(0), sampledVelocities.at(1)), displacement));
		particles.push_back(Particle(time, std::max(sampledVelocities.at(0), sampledVelocities.at(1)), displacement));
	}
	else {
		particles.push_back(Particle(time, velocity, displacement));
	}
}

double BirthSiteOld::getTime() {
	return time;
}

double BirthSiteOld::getDisplacement() {
	return displacement;
}