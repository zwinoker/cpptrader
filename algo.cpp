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
	// sm.show();
	p.show_orderbook();
}




