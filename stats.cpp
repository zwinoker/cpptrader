/* Implementation of stats.h */

#include "stats.h"
#include <math.h>
#include <algorithm>

#include <time.h>
#include <random>

#include <iostream>

/* Calculate mean of a std::vector of doubles */
double mean(std::vector<double> data) {
	double mean = 0.0;
	int size = data.size();

	for (int i = 0; i < size; i++) {
		mean += data[i];
	} 

	return mean/size;
}

/* Calculate standard deviation of a std::vector of doubles */
double Stats::stdv(std::vector<double> data, double mean) {
	double stdv = 0.0;
	int size = data.size();
	for (int i = 0; i < size; i++) {
		stdv += (data[i] - mean) * (data[i] - mean);
	}
	stdv /= (size - 1);
	stdv = sqrt(stdv);
	return stdv;
}

/* Given std::vector of doubles, returns std::vector of intervals between each successive element. */
std::vector<double> Stats::intervals(std::vector<double> data) {
	std::vector<double> intervals;

	for (int i = 0; i < data.size() - 2 ; i++) {
		intervals.insert(intervals.begin(),data[i] - data[i+1]);
	}

	return intervals;
}

/* Implements a maximum likelihood estimator for the normal distribution. Uses naive MC to 
	guess and improve the mean and standard deviation parameters. */
boost::math::normal_distribution<> Stats::normal_ml_fit(std::vector<double> jumps) {
	int mcs = 10000;
	double paramnum = 0.0;
	int numdatapts = jumps.size();

	double mean, bestmean, score, bestscore = 0.0;
	double std, beststd = 1.0;
	boost::math::normal_distribution<> testdist(mean, std);
	boost::math::normal_distribution<> bestdist(bestmean, beststd);

	/* For generating random numbers for the monte carlo bit */
	std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.001, 1.0);

    std::cout << "test 1" << std::endl;


	/* Add up likelihood for first guess distribution */
	for(int i = 0; i < numdatapts; i++) {
		bestscore += log(pdf(bestdist, jumps[i]));
	}

	/* Naive monte carlo approach to find best set of parameters by maximizing likelihood function */
	for (int i = 0; i < mcs; i++) {
		paramnum = dist(mt);
		try {
			/* Generate new mean parameter in [-1.0,1.0] */
			if (paramnum > 0.5) {
				mean = dist(mt);
				if (dist(mt) < 0.5) {
					mean *= -1.0;
				}
				testdist = boost::math::normal_distribution<>(mean, beststd);
			}

			/* Generate new std parameter in [0.001, 1.0] */
			else {
				std = dist(mt);
				testdist = boost::math::normal_distribution<>(bestmean, std);
			}	
		
			/* Add up likelihood for new distribution */
			for(int i = 0; i < numdatapts; i++) {

				if ((boost::math::isnan)(pdf(testdist, jumps[i]))) {
					std::cout << "ERROR: pdf was NAN" << std::endl;
				}
				else {
					score += log(pdf(testdist, jumps[i]));
				}	
			}
		}
		catch (const std::exception& e) {
			std::cout << "ERROR: " << e.what() << std::endl;
		}

		/* If likelihood is greater, than keep new distribution */
		if (score > bestscore) {
			bestdist = testdist;
			bestscore = score;
			bestmean = mean;
			beststd = std;
		}

		/* Reset test score */
		score = 0.0;

	}
	
	return bestdist;
}

/* Uses the given distribution parameters to generate a new set of price jumps. The average jump is then returned.  */
double Stats::mc_future_price_change(double mean, double std, int numsteps) {

	int numsims = 10000;
	double avgjump, totalavgjump = 0.0;

	/* For generating random numbers for the monte carlo bit */
	std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<double> dist(mean, std);

    /* Generate numsims MC paths. Then compute the avgerage increase for each. Then take the average of the average increases. 
    	Return this last value. */
    for (int j = 0; j < numsims; j++) {

    	for (int i = 0; i < numsteps; i++) {
    		avgjump += dist(mt);
    	}
    	avgjump /= numsteps;
    	totalavgjump += avgjump;
    	avgjump = 0.0;
    }
    
    totalavgjump /= numsims;

    return totalavgjump;
}




