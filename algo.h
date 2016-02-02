/* The algo object is responsible for determining which trades to execute. 
	It therefore has access to the stock_market and current portfolio. */

#ifndef ALGOCLASS
#define ALGOCLASS

#include <vector>
#include <string>
#include <map>
#include <tuple>

#include "types.h"
#include "Stock_Market.h"
#include "portfolio.h"
#include "order.h"

#include "boost_dt_extensions.h"
#include "statspackage.h"
//#include "stats.h"


class Algo {
	public:

		/* Algo parameters */
		int max_ticks; /* Maximum number of time steps this algo will consider */
		int frequency; /* Number of minutes between each data collection */
		int tradefrequency; /* Actually trade at this frequency, eval() is called */

		/* Constructors */
		Algo(); /* Default */
		Algo (int, int, int); /* Use when the above parameters are available */
	
		/* Order execution methods*/
		void sell_all(Portfolio&, Stock_Market&); /* Used to liquidate portfolio at the end of the day */
		void execute_order_book(Stock_Market&, Portfolio&); /* Checks if any outstanding orders should be executed. Is called every frequency minutes */

		/* Parent method. Interacts with other objects via this method. */
		void eval(Stock_Market&, Portfolio&);


};

#endif
