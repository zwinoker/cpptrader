/* The portfolio object holds the current assets and the current order book as well as methods for executing orders. */

#ifndef PORTFOLIOCLASS
#define PORTFOLIOCLASS

#include <map>
#include <vector>
#include <string>

#include "types.h"
#include "order.h"

class Portfolio {
	public:
		/* Portofolio attributes */
		double cash;
		std::map<std::string, int> holdings;
		std::string savefile, orderhistory;
		std::vector<Order> orderbook;
		
		/* Constructors */
		Portfolio();
		Portfolio (std::string); /* This is the one currently in use by the simulation object */

		/* Info methods */
		int get_cash();
		void set_cash(double);
		int num_shares(std::string);
		void show_portfolio();
		void show_orderbook();

		/* Ordering methods */
		void listorder(std::string, orderlist);
		void order(std::string, Order); /* Version of the order method that takes an order object */

		/* Recording methods */
		void save(); /* Saves current portfolio to a file */
		void record_order(std::string, Order); /* Version of the record_order method that takes an order object */
		
};

#endif