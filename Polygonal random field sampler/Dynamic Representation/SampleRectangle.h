#pragma once
#include "Dynamic Graph.h"

class SampleRectangle {
private:
	double width, height;
	std::pair<double, double> rectangeShape(double time) const;
	std::pair<double, double> boundaryIntersection(double p, double theta) const;

	std::default_random_engine sampleBoundaryp, sampleBoundarytheta, sampleInteriorwidth, sampleinteriorheight, sampleRemoveSite, sampleChooseAction;

public:
	DynamicGraph currGraph;

	SampleRectangle(double width, double height);
	void samplePRF(int iterations);
	void samplePRF(int iterations, double (*potentialFunction)(DynamicGraph&));
	void sampleBoundary();
	void sampleInterior();
	void RemoveBirthSite();
	void drawGraph();
};