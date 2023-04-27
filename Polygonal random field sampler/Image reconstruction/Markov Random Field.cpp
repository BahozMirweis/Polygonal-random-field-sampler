#include "Markov Random Field.h"
#include <math.h>

MarkovRandomField::MarkovRandomField(double width, double height, double sigma, double delta, double mu) : width{ width }, height{ height }, sigma{ sigma }, uniformDist(0, 1), delta{delta}, 
mu{mu} {
	for (int i = 0; i < height; i++) {
		std::vector<Site> row{};
		std::vector<Site> imageRow{};
		for (int j = 0; j < width; j++) {
			row.push_back(Site{ 1, 1, 1, 1, 0 });
			imageRow.push_back(Site{ 1, 1, 1, 1, 0 });
		}

		sites.push_back(row);
		image.push_back(row);
	}
}

double MarkovRandomField::localCharacteristic(int row, int col, double u) {
	double sum = 0;
	if (row != 0) {
		sum += phi(u/valueScaling - sites[row - 1][col].value/valueScaling) * sites[row][col].down;
	}

	if (row != height - 1) {
		sum += phi(u/valueScaling - sites[row + 1][col].value/valueScaling) * sites[row][col].up;
	}

	if (col != 0) {
		sum += phi(u / valueScaling - sites[row][col-1].value / valueScaling) * sites[row][col].left;
	}

	if (col != width - 1) {
		sum += phi(u / valueScaling - sites[row][col + 1].value / valueScaling) * sites[row][col].right;
	}

	sum *= 1.5;

	sum += simpleBlur(row, col, u);

	return sum;
}

double MarkovRandomField::simpleBlur(int row, int col, double u) {
	double sum = 0;
	
	for (int i = row-1; i <= row + 1; ++i) {
		if (i == -1 || i ==height) {
			continue;
		}

		for (int j = col-1; j <=col + 1; j++) {
			if (j == -1 || j == width) {
				continue;
			}
			sum += SimpleBlurMatrix(i, j, row, col, u);
		}
	}

	return sqrt(sum)/(2*sigma);
}

double MarkovRandomField::SimpleBlurMatrix(int i, int j, int row, int col, double u) {
	double sum = 0;

	for (int k = i - 1; k <= i + 1; ++k) {
		for (int l = j - 1; l <= j + 1; ++l) {
			if (k < 0 || k >= height || l < 0 || l >= width) {
				continue;
			}

			if (k == i && j == l) {
				if (k == row && l == col) {
					sum += pow(-mu + image[row][col].value / valueScaling - 0.5 * u / valueScaling, 2);
				}
				else {
					sum += pow(-mu + image[row][col].value / valueScaling - 0.5 * sites[k][l].value / valueScaling, 2);
				}
			}
			else {
				if (k == row && l == col) {
					sum += pow(-mu + image[row][col].value / valueScaling - 0.0625 * u / valueScaling, 2);
				}
				else {
					sum += pow(-mu + image[row][col].value / valueScaling - 0.0625 * sites[k][l].value / valueScaling, 2);
				}
			}
		}
	}

	return sum;
}

double MarkovRandomField::sampleFromLocalCharacteristic(int row, int col) {
	double norm = 0;
	std::vector<double> unNormalisedProbs{};
	for (int colour = 0; colour <= 255; ++colour) {
		double val = localCharacteristic(row, col, colour);
		norm += exp(-val);
		unNormalisedProbs.push_back(exp(-val));
	}

	double prob = uniformDist(unifEng);

	double cumProb = 0;

	for (int i = 0; i <= 255; ++i) {
		cumProb += unNormalisedProbs[i];
		double a = -log(norm) + log(cumProb);
		if (log(prob) >= -log(norm) - log(cumProb)) {
			if (i != 0) {
				return i;
			}

			return i;
		}
	}

	return 255;
}

void MarkovRandomField::gibbsSampler(int iterations) {
	for (int i = 0; i < iterations; ++i) {
		for (int row = 0; row < height; ++row) {
			for (int col = 0; col < width; ++col) {
				sites[row][col].value = sampleFromLocalCharacteristic(row, col);
			}
		}
	}
}

void MarkovRandomField::resetSiteBoundaries() {
	for (std::vector<Site>& row : sites) {
		for (Site& site : row) {
			site.down = 1;
			site.left = 1;
			site.right = 1;
			site.up = 1;
		}
	}
}

double MarkovRandomField::PRFPotentialFunction() {
	double sum = 0;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (i != height - 1) {
				sum += phi(sites[i + 1][j].value/valueScaling - sites[i][j].value/valueScaling) * sites[i][j].up;
			}

			if (j != width - 1) {
				sum += phi(sites[i][j + 1].value/valueScaling - sites[i][j].value/valueScaling) * sites[i][j].right;
			}
		}
	}

	return sum*1.5;
}

double MarkovRandomField::phi(double u) {
	return 1 - 2 / (1 + (u / delta) * (u / delta));
}