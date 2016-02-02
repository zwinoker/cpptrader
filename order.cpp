#include "order.h"
#include <iostream>

using namespace std;

/* Empty constructor */
Order::Order () {symbol = "----";}


/* Constructor for market orders */
Order::Order (string sym, bool ismarket, int amount, double pr) {
	symbol = sym;
	ismarketorder = ismarket;
	num = amount;
	price = pr;

	/* Placeholders to avoid issues if you try to access this stuff for a market order*/
	expirationdate = "----------";
	executeatexpiry = false;
	stoploss = 0.0;
	execute_above_price = false;
}


/* Constructor for limit orders */
Order::Order (string sym, bool ismarket, int amount, double pr, string expdate, bool exec_at_exp, double sl, bool abovebelow){
	symbol = sym;
	ismarketorder = ismarket;
	num = amount;
	price = pr;
	expirationdate = expdate;
	executeatexpiry = exec_at_exp;
	stoploss = sl;
	execute_above_price = abovebelow;
}

/* Prints order details to console. Used mostly for printing out orderbook*/
const void Order::show() const {
	cout << "|  " << symbol << "  |  " << num << "  |  " << price << "  |  " << expirationdate << "  |  " << executeatexpiry << "  |  " << stoploss << "  |  " << execute_above_price << "  |" << endl;

}