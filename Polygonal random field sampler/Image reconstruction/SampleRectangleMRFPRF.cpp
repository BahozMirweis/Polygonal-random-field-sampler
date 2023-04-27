#include "SampleRectangleMRFPRF.h"
#include <matplot/matplot.h>

namespace plt = matplot;

SampleRectangleMRFPRF::SampleRectangleMRFPRF(double width, double height, std::string imageLoc) : width{ width }, height{ height }, sampleBoundaryp{ std::default_random_engine(std::time(0)) }, sampleBoundarytheta{ std::default_random_engine(2* std::time(0)) },
sampleInteriorwidth{ std::default_random_engine(3* std::time(0)) }, sampleinteriorheight{ std::default_random_engine(4* std::time(0)) }, sampleRemoveSite{ std::default_random_engine(5* std::time(0)) },
sampleChooseAction{ std::default_random_engine(6* std::time(0)) }, image{ cv::imread(imageLoc, cv::IMREAD_GRAYSCALE) } {
	currGraph = DynamicTessellation(width, height);

	mrfHeight = image.rows;
	mrfWidth = image.cols;
	mrf = MarkovRandomField(mrfWidth, mrfHeight, 3, 0.2, 0);

	uint8_t* pixelPtr = (uint8_t*)image.data;
	int cn = image.channels();
	
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{	
			mrf.image[i][j].value = pixelPtr[i * image.cols * cn + j * cn + 0];
			mrf.sites[i][j].value = pixelPtr[i * image.cols * cn + j * cn + 0];
		}
	}
}

std::pair<double, double> SampleRectangleMRFPRF::boundaryIntersection(double p, double theta) const {
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


void SampleRectangleMRFPRF::sampleBoundary() {
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

void SampleRectangleMRFPRF::sampleInterior() {
	std::uniform_real_distribution<double> sampleWidth(0, width);
	std::uniform_real_distribution<double> sampleHeight(0, height);

	double x = sampleWidth(sampleInteriorwidth);
	double y = sampleHeight(sampleinteriorheight);

	currGraph.addBirthsite(BirthSite(y, x, false));
}

void SampleRectangleMRFPRF::RemoveBirthSite() {
	int totalBirthSites{ static_cast<int>(currGraph.birthSites.size()) };

	std::uniform_int_distribution<int> sampleSite(0, totalBirthSites - 1);
	int toRemove = sampleSite(sampleRemoveSite);

	currGraph.removeParticle(toRemove);
}

void SampleRectangleMRFPRF::samplePRF(int iterations) {
	int n = 0;

	std::uniform_real_distribution<double> sampleProb(0, 1);
	double perim{ 2 * width + 2 * height };
	double area{ width * height };

	DynamicTessellation minimumGraph{ currGraph };
	double minimumPotential{ PRFpotentialFunction(minimumGraph) };

	auto t_start = std::chrono::high_resolution_clock::now();

	while (n < iterations) {
		DynamicTessellation prevGraph{ currGraph };

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

		//drawGraph();
		//plt::cla();

		prob = sampleProb(sampleChooseAction);
		double first = PRFpotentialFunction(prevGraph);
		double second = PRFpotentialFunction(currGraph);
		double acceptanceProb = first - second;

		if (std::log(prob) > acceptanceProb) {
			currGraph = prevGraph;
		}
		else {
			if (second < minimumPotential) {
				minimumGraph = currGraph;
				minimumPotential = second;
			}
		}

		++n;

		if (n % 1000 == 0) {
			std::cout << "PRF iteration: " << n << '\n';
			auto t_end = std::chrono::high_resolution_clock::now();
			std::cout << "Iteration time: " << std::chrono::duration<double, std::milli>(t_end - t_start).count() / 1000 << '\n';
			t_start = std::chrono::high_resolution_clock::now();
		}
	}

	//currGraph = minimumGraph;

	minimumGraph.drawTessellation();
}

void SampleRectangleMRFPRF::removeMRFEdges(DynamicTessellation& g) {
	

	for (const std::shared_ptr<Vertex>& v : g.vertices) {
		if (!v->parent.expired()) {
			MRFRemoveEdgesHelper(v->parent.lock()->displacement, v->parent.lock()->time, v->displacement, v->time);
		}

		if (!v->parent2.expired()) {
			MRFRemoveEdgesHelper(v->parent2.lock()->displacement, v->parent2.lock()->time, v->displacement, v->time);
		}
	}
}

void SampleRectangleMRFPRF::MRFRemoveEdgesHelper(double displacement1, double time1, double displacement2, double time2) {
	double scalingFactorX = mrfWidth / width, scalingFactorY = mrfHeight / height;

	time1 = time1 * scalingFactorX;
	displacement1 = displacement1 * scalingFactorY;
	time2 = time2 * scalingFactorX;
	displacement2 = displacement2 * scalingFactorY;

	double m = (displacement1 - displacement2) / (time1 - time2);
	double c = displacement1 - time1 * m;

	for (int t = ceil(time1); t <= time2; ++t) {
		int dPos = floor(m * t + c);
		if (t >= 0 && t < mrfWidth && dPos >= 0 && dPos < mrfHeight - 1) {
			mrf.sites[dPos][t].up = 0;
			mrf.sites[dPos + 1][t].down = 0;
		}
	}

	for (int d = ceil(min(displacement1, displacement2)); d <= max(displacement1, displacement2); ++d) {
		int tPos = floor((d - c) / m);
		if (tPos >= 0 && tPos < mrfWidth - 1 && d >= 0 && d < mrfHeight) {
			mrf.sites[d][tPos].right = 0;
			mrf.sites[d][tPos + 1].left = 0;
		}
	}
}

void SampleRectangleMRFPRF::SampleMRFPRF(int iterations, int subIterations) {
	int n = 0;

	while (n < iterations) {
		std::cout << "n = " << n << '\n';
		auto t_start = std::chrono::high_resolution_clock::now();

		samplePRF(subIterations);
		mrf.gibbsSampler(1);
		auto t_end = std::chrono::high_resolution_clock::now();
		std::cout << "Iteration time: " << std::chrono::duration<double, std::milli>(t_end - t_start).count() / 1000 << '\n';
		++n;

		if (n % 10 == 0) {
			saveImage(n);
		}
	}

	saveImage(1234567);
}

double SampleRectangleMRFPRF::PRFpotentialFunction(DynamicTessellation& g) {
	mrf.resetSiteBoundaries();
	removeMRFEdges(g);
	return mrf.PRFPotentialFunction();
}

void SampleRectangleMRFPRF::saveImage(int num) {
	cv::Mat imageOut(mrfHeight, mrfWidth, CV_8UC3);
	for (int i = 0; i < mrfHeight; ++i) {
		for (int j = 0; j < mrfWidth; ++j) {
			imageOut.at<cv::Vec3b>(cv::Point(j, i)) = cv::Vec3b(mrf.sites[i][j].value, mrf.sites[i][j].value, mrf.sites[i][j].value);
		}
	}

	cv::imwrite("C:\\Users\\bahoz\\source\\repos\\Polygonal random field sampler\\x64\\Debug\\images\\Cat blur output " + std::to_string(num) + ".bmp", imageOut);
}