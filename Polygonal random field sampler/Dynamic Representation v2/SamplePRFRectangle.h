#pragma once
#include "Dynamic Tessellation.h"

class SamplePRFRectangle {
private:
	double width, height;
	std::pair<double, double> boundaryIntersection(double p, double theta) const;

	std::default_random_engine sampleBoundaryp, sampleBoundarytheta, sampleInteriorwidth, sampleinteriorheight, sampleRemoveSite, sampleChooseAction;

public:
	DynamicTessellation currGraph;

	SamplePRFRectangle(double width, double height);
	void samplePRF(int iterations);
	void samplePRF(int iterations, double (*potentialFunction)(DynamicTessellation&));
	void sampleBoundary();
	void sampleInterior();
	void RemoveBirthSite();
};