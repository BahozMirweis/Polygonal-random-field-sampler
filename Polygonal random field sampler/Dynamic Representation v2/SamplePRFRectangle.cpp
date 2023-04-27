#include "SamplePRFRectangle.h"
#include <matplot/matplot.h>

namespace plt = matplot;

SamplePRFRectangle::SamplePRFRectangle(double width, double height) : width{ width }, height{ height }, sampleBoundaryp{ std::default_random_engine(111111) }, sampleBoundarytheta{ std::default_random_engine(222222) },
sampleInteriorwidth{ std::default_random_engine(333333) }, sampleinteriorheight{ std::default_random_engine(444444) }, sampleRemoveSite{ std::default_random_engine(55555) },
sampleChooseAction{ std::default_random_engine(666666) } {
	currGraph = DynamicTessellation(width, height);
}

std::pair<double, double> SamplePRFRectangle::boundaryIntersection(double p, double theta) const {
	// Take the intersection with minimum time.

	double grad = -1 / std::tan(theta);
	double intersect = p / std::sin(theta);

	double topX = (height - intersect) / grad;
	double bottomX = -intersect / grad;
	double leftY = intersect;

	if (leftY >= 0 && leftY <= height) {
		return std::pair(0, leftY);
	}
	else if (topX < bottomX && topX >= 0 && topX <= width) {
		return std::pair(topX, height);
	}
	else if (bottomX < topX && bottomX >= 0 && bottomX <= width) {
		return std::pair(bottomX, 0);
	}
	else {
		return std::pair(-1, -1);
	}

}

void SamplePRFRectangle::sampleBoundary() {
	double radius = std::sqrt(width * width / 4 + height * height / 4);
	// Set of lines is bounded by rectangle with dimensions [0, pi] * [-radius, radius].
	// Take two uniform samples of this set and check if it intersects the rectangle.
	// Take the interior point of this intersection.

	std::uniform_real_distribution<double> pSample(-radius, radius);
	std::uniform_real_distribution<double> thetaSample(0, std::_Pi);
	double p, theta, x, y;

	do {
		p = pSample(sampleBoundaryp);
		theta = thetaSample(sampleBoundarytheta);

		std::pair<double, double> collision{ boundaryIntersection(p, theta) };
		x = collision.first;
		y = collision.second;

	} while (x < 0);

	double initVelocity = -1 / std::tan(theta);

	currGraph.addBirthsite(BirthSite(y, x, true, initVelocity));

}

void SamplePRFRectangle::sampleInterior() {
	std::uniform_real_distribution<double> sampleWidth(0, width);
	std::uniform_real_distribution<double> sampleHeight(0, height);

	double x = sampleWidth(sampleInteriorwidth);
	double y = sampleHeight(sampleinteriorheight);

	currGraph.addBirthsite(BirthSite(y, x, false));
}

void SamplePRFRectangle::RemoveBirthSite() {
	int totalBirthSites{ static_cast<int>(currGraph.birthSites.size()) };

	std::uniform_int_distribution<int> sampleSite(0, totalBirthSites - 1);
	int toRemove = sampleSite(sampleRemoveSite);

	currGraph.removeParticle(toRemove);
}

void SamplePRFRectangle::samplePRF(int iterations) {
	int n = 0;

	std::uniform_real_distribution<double> sampleProb(0, 1);
	double perim{ 2 * width + 2 * height };
	double area{ width * height };

	while (n < iterations) {
		int totalBirthSites{ static_cast<int>(currGraph.birthSites.size()) };

		double denom{ perim + area + totalBirthSites };

		double prob{ sampleProb(sampleChooseAction) };

		if (prob <= area / denom) {
			sampleInterior();
		}
		else if (prob <= (area + perim) / denom) {
			sampleBoundary();
		}
		else if (totalBirthSites != 0) {
			RemoveBirthSite();
		}
		
		if (n >= 28) {
			//currGraph.drawTessellation();
			//currGraph.drawAllVertices();
		}

		++n;
	}
}

void SamplePRFRectangle::samplePRF(int iterations, double (*potentialFunction)(DynamicTessellation&)) {
	int n = 0;

	std::uniform_real_distribution<double> sampleProb(0, 1);
	double perim{ 2 * width + 2 * height };
	double area{ width * height };

	while (n < iterations) {

		DynamicTessellation prevGraph{ currGraph };

		//std::cout << potentialFunction(currGraph) << '\n';

		int totalBirthSites{ static_cast<int>(currGraph.birthSites.size()) };

		double denom{ perim + area + totalBirthSites };

		double prob{ sampleProb(sampleChooseAction) };

		if (prob <= area / denom) {
			sampleInterior();
		}
		else if (prob <= (area + perim) / denom) {
			sampleBoundary();
		}
		else if (totalBirthSites != 0) {
			RemoveBirthSite();
		}

		prob = sampleProb(sampleChooseAction);
		double acceptanceProb = potentialFunction(prevGraph) - potentialFunction(currGraph);

		if (std::log(prob) > acceptanceProb) {
			currGraph = prevGraph;
		}

		++n;

	}
}