#include "Metropolis Hastings.h"
#include <random>
#include <vector>
#include <cmath>
#include <armadillo>
#include <cstdlib>  
#include <ctime>

namespace MetropolisHastings {
	static std::default_random_engine Normalgenerator(std::time(0)), uniformGenerator(std::time(0));

	double sample1D(const std::function<double(double)>& logDensity, const double& initialValue, const double& variance, const int& iterations) {
		double prev{ initialValue };

		std::normal_distribution<double> normalDistribution(0, std::sqrt(variance));
		std::uniform_real_distribution<double> uniformDistribution(0, 1);

		for (int i = 0; i < iterations; ++i) {
			double move{ prev + normalDistribution(Normalgenerator) };

			double logRneg{ logDensity(prev) - logDensity(move) };

			double prob{ std::max(logRneg, 0.0) };

			double acceptance{ uniformDistribution(uniformGenerator) };
			double logAcceptance{ -std::log(acceptance) };
			
			if (logAcceptance >= prob) {
				prev = move;
				//values.push_back(move);
			}
			else {
				//values.push_back(prev);
			}
		};
		

		return prev;
	}

	arma::vec sample2D(const std::function<double(arma::vec)>& logDensity, const arma::vec& initialValue, const arma::mat& covariance, const int& iterations) {
		//std::vector<arma::vec> data{};
		arma::vec prev{ initialValue };
		arma::mat diffs{ arma::mvnrnd(arma::vec({ 0.0, 0.0 }), covariance, iterations) };

		std::uniform_real_distribution<double> uniformDistribution(0, 1);

		for (int i = 0; i < iterations; ++i) {
			arma::vec move{ prev + diffs.col(i) };

			double logRneg{ logDensity(prev) - logDensity(move) };

			double prob{ std::max(logRneg, 0.0) };

			double acceptance{ uniformDistribution(uniformGenerator) };
			double logAcceptance{ -std::log(acceptance) };

			if (logAcceptance >= prob) {
				prev = move;
				//data.push_back(move);
			}
			else {
				//data.push_back(prev);
			}
		}

		return prev;
	}

	std::vector<double> sample1DTest(const std::function<double(double)>& logDensity, const double& initialValue, const double& variance, const int& iterations) {
		std::vector<double> values{ };
		double prev{ initialValue };

		std::default_random_engine Normalgenerator, uniformGenerator;
		std::normal_distribution<double> normalDistribution(0, std::sqrt(variance));
		std::uniform_real_distribution<double> uniformDistribution(0, 1);

		for (int i = 0; i < iterations; ++i) {
			double move{ prev + normalDistribution(Normalgenerator) };

			double logRneg{ logDensity(prev) - logDensity(move) };

			double prob{ std::max(logRneg, 0.0) };

			double acceptance{ uniformDistribution(uniformGenerator) };
			double logAcceptance{ -std::log(acceptance) };

			if (logAcceptance >= prob) {
				prev = move;
				values.push_back(move);
			}
			else {
				values.push_back(prev);
			}
		};


		return values;
	}
};