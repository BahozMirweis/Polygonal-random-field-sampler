#include <cmath>
#include "Metropolis Hastings.h"
#include <chrono>
#include <armadillo>
#include "Dynamic Graph.h"
#include "SampleRectangle.h"
#include <matplot/matplot.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "SampleRectangleMRFPRF.h"
#include <opencv2/core/hal/interface.h>
#include "Dynamic Tessellation.h"
#include "SamplePRFRectangle.h"

namespace plt = matplot;

int main()
{
    time_t time = std::time(0);
    std::cout << time << '\n';
    arma::arma_rng::set_seed(time);

    cv::setBreakOnError(true);
    SampleRectangleMRFPRF s{ SampleRectangleMRFPRF(21.6, 25.5, "C:\\Users\\bahoz\\source\\repos\\Polygonal random field sampler\\x64\\Debug\\black cat blur.bmp") };
 
    s.SampleMRFPRF(5, 500);

    s.currGraph.drawTessellation();

    std::cout << s.PRFpotentialFunction(s.currGraph);

    return 0;
}