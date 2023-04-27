#pragma once
#include <vector>
#include <armadillo>
#include <functional>

namespace MetropolisHastings {
	double sample1D(const std::function<double(double)>& logDensity, const double& initialValue, const double& variance, const int& iterations);
	arma::vec sample2D(const std::function<double(arma::vec)>& logDensity, const arma::vec& initialValue, const arma::mat& covariance, const int& iterations);
	std::vector<double> sample1DTest(const std::function<double(double)>& logDensity, const double& initialValue, const double& variance, const int& iterations);
};