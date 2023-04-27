#pragma once
#include <vector>
#include <random>

struct Site {
	int left; 
	int right;
	int up;
	int down;
	int value;
};

class MarkovRandomField {
private:
	double width, height;
	double sigma;
	double mu;
	double delta;
	double valueScaling{50};
	std::default_random_engine unifEng;
	std::uniform_real<double> uniformDist;

	double phi(double u);
	
	// blurring at site i, j
	//1/(2*sigma)||mu - phi^{-1}(y_{i, j}, sum_{<k, l>} H(i - k, j - l) x_{k, l})||
	//double (*blurring) (int i, int j);

	// interaction between edges. Commonly 1- 2/(1+(u/delta)^{2})

public:
	std::vector<std::vector<Site>> sites, image;

	MarkovRandomField(double width, double height, double sigma, double delta, double mu);

	MarkovRandomField() = default;

	MarkovRandomField& operator=(const MarkovRandomField&) = default;

	double localCharacteristic(int row, int col, double u);
	double simpleBlur(int row, int col, double u);
	double SimpleBlurMatrix(int i, int j, int row, int col, double u);

	double sampleFromLocalCharacteristic(int row, int col);

	void gibbsSampler(int iterations);

	void resetSiteBoundaries();

	double PRFPotentialFunction();
};