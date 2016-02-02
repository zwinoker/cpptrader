#include "algo.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <functional>


using namespace std;
using namespace boost::math;
using namespace boost::posix_time;
using namespace boost::gregorian;


/*Default constructor*/
Algo::Algo() {
	max_ticks = 25;
	frequency = 1;
	tradefrequency = 5;


}

/* Customize parameters */
Algo::Algo(int mt, int freq, int trfreq) {
	max_ticks = mt;
	frequency = freq;
	tradefrequency = trfreq;
}




/* At the end of the day, we liquidate the portfolio using this method. */
void Algo::sell_all(Portfolio &p, Stock_Market &sm) {
	std::map<std::string, int> assets = p.holdings;

	std::map<std::string, int>::iterator it;
	for (it = assets.begin(); it != assets.end(); ++it) {
		if ((*it).second != 0) {
			p.order(sm.workingtime, Order((*it).first, true, -1*(*it).second, sm.get_info((*it).first,0)));
		}
	}		
}

/* Run every minute to see if we need to execute or expire outstanding orders */
void Algo::execute_order_book(Stock_Market &sm, Portfolio &p) {

    double currentprice = 0.0;
	vector<Order>::iterator it;
	ptime currenttime(time_from_string(sm.workingtime));

	for (it = p.orderbook.begin(); it != p.orderbook.end(); ++it) {
		Order order = (*it);
		currentprice = sm.get_info(order.symbol, 0);
		ptime executiondate(time_from_string((*it).expirationdate));

		/* If order is past expiration date, then we either execute it or just delete it from the order book */
		if (currenttime >= time_from_string(order.expirationdate)) {
			if (order.executeatexpiry) {
				p.order(sm.workingtime, Order(order.symbol, true, order.num, currentprice));
			}
			(*it).num = 0;

		}

		/* Check other order execution conditions */
		else if ((order.execute_above_price && currentprice >= order.price) || 
			    (!(order.execute_above_price) && currentprice <= order.price) || (currentprice <= order.stoploss)) {

			p.order(sm.workingtime, Order(order.symbol, true, order.num, currentprice));
			(*it).num = 0;
		}

		/* Else do nothing */

	}

	/* Removes executed orders, which we've marked by making their order.num == 0. */
	p.orderbook.erase(remove_if(p.orderbook.begin(),p.orderbook.end(), [](Order o) {return (o.num == 0);}),p.orderbook.end());
}

		

/* Evaluates current stock market to issue buy/sell orders. This method is called from simulation object every tradefrequency minutes */
void Algo::eval(Stock_Market &sm, Portfolio &p) {
	//norm_ml(sm, p, max_ticks, transactioncost, maxriskablecapital, tradefrequency);
	sm.show();
	p.show_orderbook();
}





// /* Uses MCML fit and filter and MC path generation and analysis to decide buy/sell orders */
// void norm_ml(Stock_Market &sm, Portfolio &p, int max_ticks, double transcost, double maxrisk, int tradefrequency) {

// 	vector<string>::iterator symb;
// 	int numshares, bestnumshares = 0;
// 	double price, avg, bestavg, std, beststd, shape, bestprice, prob, bestprob, score, bestscore, avg_jump, best_avg_jump = 0.0;
// 	double spendablecash = p.cash * maxrisk;

// 	vector<double> pdata;
// 	vector<double> jumps;
// 	vector<tuple<string, double, double> > goodstocks;

// 	string symbol, selldate;
// 	string bestsymbol = "----";

// 	/* MCML fit and filtering on each stock.  */
// 	for (symb = sm.symbol_list.begin(); symb != sm.symbol_list.end() ; ++symb) {

// 		symbol = *symb;

// 		if (sm.check_date(symbol)) { /* Ensure that the symbol's most recent tick is from the current time. Helps avoid bad data. */
// 			price += sm.get_info(symbol, 0);
// 			pdata = sm.get_info_vector(symbol,max_ticks/tradefrequency,0,tradefrequency);
// 			jumps = intervals(pdata);
// 			avg = mean(jumps);
// 			std = stdv(jumps, avg);

// 			try {
// 				if (std != 0.0) {	

// 					boost::math::normal_distribution<> testdist = normal_ml_fit(jumps);
// 					prob = 1.0 - cdf(testdist, 0.03);
// 					numshares = floor(spendablecash/price);
// 					transcost = 0.03 * numshares; /* Assumes 0.03 centers per share commission. As per Interactive Brokers contract. */

// 					if (prob > 0.60 && avg > 0.0 && (numshares * 0.03 >= transcost)) {
// 						goodstocks.push_back(tuple<string, double, double>(symbol, mean(testdist), standard_deviation(testdist)));
// 					}
// 				}
// 			}
// 			catch (const std::exception& e) {
// 				std::cout << "ERROR in ML fit: " << e.what() << std::endl;
// 			}
			
// 			price = 0.0;
// 		}
// 	}

// 	 MC path generation on each stock in goodstocks. Then pick the stock whose paths generated the highest average price increase. 
// 	try {
// 		vector<tuple<string, double, double> >::iterator gs;
// 		for (gs = goodstocks.begin(); gs != goodstocks.end(); ++gs) {

// 			avg_jump = mc_future_price_change(get<1>(*gs), get<2>(*gs), 3 * max_ticks/tradefrequency);

// 			if ((avg_jump > best_avg_jump) ) {
// 					bestsymbol = get<0>(*gs);
// 			}
// 		}
// 	}
// 	catch (const std::exception& e) {
// 				std::cout << "ERROR in MC path generation: " << e.what() << std::endl;
// 	}

// 	try {
// 	/* Buy the best stock if we have the money. 800000 is hardcoded in as 20% of the initial investment of 100,000. */
// 		if (bestsymbol != "----" ) {
// 			/* Find the price and number of shares we can buy for the stock that the MC filter said was best. */
// 			bestprice = sm.get_info(bestsymbol, 0);
// 			bestnumshares = floor(spendablecash/bestprice);
// 			transcost = 0.03 * bestnumshares; /* Assumes 0.03 centers per share commission. As per Interactive Brokers contract. */

// 			pdata = sm.get_info_vector(bestsymbol,max_ticks/tradefrequency,0,tradefrequency);
// 			jumps = intervals(pdata);
// 			avg = mean(jumps);
// 			std = stdv(jumps, avg);

// 			if ((p.cash - bestnumshares*bestprice >= 80000)) {
// 				Order buy = Order(bestsymbol, true, bestnumshares, bestprice);

// 				selldate = date_time_to_string(time_from_string(sm.workingtime) + minutes(5*tradefrequency));

// 				//Order sell = Order(bestsymbol, false, -1*bestnumshares, bestprice + 0.1, selldate, true, bestprice - 0.5, true);
// 				Order sell = Order(bestsymbol, false, -1*bestnumshares, bestprice + 0.03, selldate, true, bestprice - 2.0 * std, true);
				
// 				p.cash -= 2.0*transcost;
// 				p.order(sm.workingtime, buy);
// 				p.order(sm.workingtime, sell);
// 			}
// 		}
// 	}
// 	catch (const std::exception& e) {
// 				std::cout << "ERROR in order execution: " << e.what() << std::endl;
// 	}

// }





