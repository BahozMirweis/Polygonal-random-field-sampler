#include "Birth Site.h"
#include <functional>
#include <armadillo>
#include "Metropolis Hastings.h"

//velocity should only be defined at boundary site.
BirthSite::BirthSite(double displacement, double time, bool boundarySite, double velocity) : displacement{ displacement }, time{ time }, boundarySite{ boundarySite } {
	if (!boundarySite) {
		std::function<double(arma::vec)> particleDensity{ [this](arma::vec velocities) {return this->particleBirthDensity(velocities.at(0), velocities.at(1)); } };

		arma::vec sampledVelocities{ MetropolisHastings::sample2D(particleDensity, arma::vec{ {0.0, 0.0} }, arma::mat{ {1, 0.5}, {0.5, 1} }, 50000) };

		// to make dynamic graph adding particles easier.
		velocities.push_back(std::min(sampledVelocities.at(0), sampledVelocities.at(1)));
		velocities.push_back(std::max(sampledVelocities.at(0), sampledVelocities.at(1)));
	}
	else {
		velocities.push_back(velocity);
	}
}

double BirthSite::getTime() const {
	return time;
}

double BirthSite::getDisplacement() const {
	return displacement;
}