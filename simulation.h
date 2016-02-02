/* The simulation class is the top level object. It contains all other objects and advances the simulation through each time step. */

#ifndef SIMULATIONCLASS
#define SIMULATIONCLASS

#include "algo.h"
#include "scraper.h"

#include "types.h"


class Simulation{
	public:
		/* Simulation parameters */
		bool live; /* If false, then simulation is a backtest on historical data. */
		std::string startdate; /* The start date and time for the backtest */
		std::string enddate; /* The end date and time for the backtest */
		std::string portfoliosavefile; /* The name of the file where the portfolio's assets are saved */
		int tz_offset; /* Number of hours difference from NYC due to time zone. (eg LA is 3) */

		/* Other objects. Get their parameters from the input file read by Simulation constructor. */
		Portfolio p;
		Algo trader;
		Stock_Market sm;
		Scraper scr;

		/* Constructor */
		Simulation();  /* Reads from input file */

		/* Use the stock_market to figure out the value of the portfolio's total assets */
		double value_portfolio(std::string);

		/* Runs the simluation according to the above parameters */
		void run_simulation(); 	
};

#endif