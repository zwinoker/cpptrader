/* Implementation of portoflio.h */

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <iomanip>
#include <iostream>
#include <fstream>
#include "portfolio.h"


using namespace std;

/* Default constructor */
Portfolio::Portfolio() {
	cash = 0.0;
	savefile = "portfolio.save";
	orderhistory = "portfolio.orderhistory";
}

/* Takes a name of a text file that contains the portfolio data.
 Format is cash on first line, then (asset,numasset) on each line after that */
Portfolio::Portfolio(string filename) {
	savefile = filename;
	fstream saved_portfolio;
	saved_portfolio.open(filename);
	string line;
	string symbolholder;
	getline(saved_portfolio, line);
	cash = stod(line);
	while (getline(saved_portfolio, symbolholder,',')){
		getline(saved_portfolio, line);
		holdings[symbolholder] = stoi(line);
	}
	saved_portfolio.close();
	orderhistory = "portfolio.orderhistory";
}

/* Writes the current portfolio to the portfolio save file. */
void Portfolio::save() {
	ofstream savehere;
	savehere.open(savefile);
	map<string,int>::const_iterator it;
	savehere << cash << endl;
	for(it= holdings.begin(); it != holdings.end(); ++it) {
		savehere<< it->first <<","<< it->second << endl;
	}
	savehere.close();
}

/* Returns the amount of cash currently held. */
int Portfolio::get_cash() {
	return cash;
}

/* Can be used to change the amount of cash currently held. */
void Portfolio::set_cash(double c) {
	cash = c;
}

/* Returns the number of shares of a given symbol that are currently held. */
int Portfolio::num_shares(string symbol) {
	return holdings[symbol];
}

/* Writes details of a given order to the orderhistory file. */
void Portfolio::record_order(string datetime, Order order) {
	ofstream writefile;
	writefile.open(orderhistory, std::fstream::app);
	string info = datetime + ", " + order.symbol + ", " + to_string(order.num) + ", " + to_string(order.price) + "\n";
    writefile << info;
	writefile.close();
}

/* Executes a single order */
void Portfolio::order(string datetime, Order order){
	if (order.symbol != "----") {

		/* If it's a market order, just execute it. */
		if (order.ismarketorder) {
			record_order(datetime,order);
			cash -= (order.num * order.price);
			holdings[order.symbol] += order.num;
		}

		/* Else if it's a limit order, add it to the orderbook */
		else {
			orderbook.push_back(order);
		}
	}
}

/* Prints the current portfolio to the console */
void Portfolio::show_portfolio() {
	map<string,int>::const_iterator it;
	cout<< endl<<"Portfolio holdings: " << endl;
	cout<< "Cash: " << cash << endl;
	for(it= holdings.begin(); it != holdings.end(); ++it) {
		cout<< it->first <<setw( 10) << it->second << endl;
	}
}

/* Prints the current order book to the console */
void Portfolio::show_orderbook() {
	vector<Order>::const_iterator it;
	cout << endl << "Current order book: " << endl;
	cout << "|  Symb  | Num | Price|  Expiration Date      |ExExp|StopLoss| AbvBelow|" << endl;
	cout << "______________________________________________________________________________" << endl;
	for(it = orderbook.begin(); it != orderbook.end(); ++it) {
		(*it).show();
	}
}



