/*	This class encompases the types of orders we can add to the portfolio's order book  */

#ifndef ORDERCLASS 
#define ORDERCLASS

#include <string>

class Order {
	public:
		/* Order details */
		std::string symbol; /* Which stock we're ordering */
		bool ismarketorder; /* Whether this is a market order or limit order */
		int num; /* Number of shares. If negative, the order is a sell order. If positive, order is a buy order*/
		double price; /* Execution price. Is just current market price if market order */

		/* Details for limit orders only */
		std::string expirationdate; /* When the order expires */
		bool executeatexpiry; /* Should we execute the order when it reaches its expiration or just discard it? */
		double stoploss; /* If below this price, sell.*/
		bool execute_above_price; /* If true, then order is executed when market price is above order price. If false, then execute below given price. */
		
		/* Constructors.*/
		Order (); /* Default */
		Order (std::string, bool, int, double); /* Market order */
		Order (std::string, bool, int, double, std::string, bool, double, bool); /* Limit order */

		/* Info methods */
		const void show() const;
};

#endif