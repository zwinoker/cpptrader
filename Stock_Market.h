/* The stock_market holds the most recent stock price data and has methods for retrieving that data. */

#ifndef STOCKMARKETCLASS
#define STOCKMARKETCLASS

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "types.h"

class Stock_Market {
	public:
		/* Properties */
		market marketdata; /* Holds recent stock market information */
		std::vector<std::string> symbol_list; /* List of symbols to monitor */
		std::string workingtime; /* The current time */
		// int current_elt;  /* Which element in the tick data array corresponds to the current time. */
		// int earliest_elt; /* Which element in the tick data array corresponds to the max_ticks tick ago. */

		/* Constructors */
		Stock_Market(); /* Default */
		/* Takes in frequency, max_ticks, start date-time, and SQL parameters*/
		Stock_Market(int, int, std::string, const char*, const char*, const char*, const char*); 

		/* Version that enables fewer sql queries. Takes in frequency, max_ticks, future_ticks, start date-time, and SQL parameters */
		//Stock_Market(int, int, int, std::string, const char*, const char*, const char*, const char*); 
		
		
		/* Information retrieval methods */
		double get_info(std::string,int); //args --> symbol, timestamp, info (0==close eg)
		std::vector<double> get_info_vector(std::string, int, int, int);
		std::vector<tick_data> get_info_matrix(std::string, int, int);
		
		/* For display */
		void show();

		/* MISC */
		bool check_date(std::string);
};

#endif