#pragma once

#pragma once
#include "Dynamic Tessellation.h"
#include "Markov Random Field.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

class SampleRectangleMRFPRF {
private:
	double width, height, mrfWidth, mrfHeight;
	std::pair<double, double> boundaryIntersection(double p, double theta) const;

	cv::Mat image;

	MarkovRandomField mrf;
	double PRFpotentialFunction();
	void MRFRemoveEdgesHelper(double displacement1, double time1, double displacement2, double time2);

	std::default_random_engine sampleBoundaryp, sampleBoundarytheta, sampleInteriorwidth, sampleinteriorheight, sampleRemoveSite, sampleChooseAction;

public:
	DynamicTessellation currGraph;

	SampleRectangleMRFPRF(double width, double height, std::string imageLoc);
	void samplePRF(int iterations);
	void sampleBoundary();
	void sampleInterior();
	void RemoveBirthSite();

	double PRFpotentialFunction(DynamicTessellation & g);

	void removeMRFEdges(DynamicTessellation& g);
	void SampleMRFPRF(int iterations, int subIterations);
	void saveImage(int num);
};