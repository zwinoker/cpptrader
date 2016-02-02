/* Useful custom statisical functions. These are general functions; they do not know about anything related to finance. Anything that
	requires access to the portfolio or stock market should be in the algo.cpp class. */

#ifndef STATSPACKAGE
#define STATSPACKAGE

#include <stdlib.h>
#include <boost/math/distributions/normal.hpp>



class Stats {
	public:
		/* Constructor */
		Stats();

		/* Basic statistics */
		double mean(std::vector<double>); /* Calculate mean of a std::vector of doubles */
		double stdv(std::vector<double> data, double mean);/* Calculate standard deviation of a std::vector of doubles */ 

		std::vector<double> intervals(std::vector<double> data); /* Given std::vector of doubles, returns std::vector of intervals between each successive element. */

		/* Implements a maximum likelihood estimator for the normal distribution. Uses naive MC to 
			guess and improve the mean and standard deviation parameters. */
		boost::math::normal_distribution<> normal_ml_fit(std::vector<double> jumps); 

		/* Uses the given distribution parameters to generate a new set of price jumps. The average jump is then returned. */
		double mc_future_price_change(double mean, double std, int numsteps);
};

#endif
