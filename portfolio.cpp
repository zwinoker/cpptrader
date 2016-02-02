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

// /* Can build a portfolio by giving it a cash value and a vector of assets */
// Portfolio::Portfolio(double c, vector<pair<string, int> > symbol_list) {
// 	cash = c;
// 	vector<pair<string, int> >::const_iterator it;
// 	for(it= symbol_list.begin(); it != symbol_list.end(); ++it) {
// 		holdings[it->first] = it->second;
// 	}
// 	savefile = "portfolio.save";
// 	orderhistory = "portfolio.orderhistory";

// }

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

// /* Old method for recording objects. Only used in live testing now */
// void Portfolio::record_order(string datetime, tuple<string,int,double> order) {
// 	ofstream writefile;
// 	writefile.open(orderhistory, std::fstream::app);
// 	string info, symbol;
// 	int num;
// 	double price;
// 	symbol = get<0>(order);
// 	num = get<1>(order);
// 	price = get<2>(order);
// 	info = datetime + ", " + symbol + ", " + to_string(num) + ", " + to_string(price) + "\n";
//     writefile << info;
// 	writefile.close();
// }

// /* Old method for executing an order. Only used in live testing now. */
// void Portfolio::order(string datetime, tuple<string,int,double> order){
// 	string symbol;
// 	int num;
// 	double price;
// 	symbol = get<0>(order);
// 	num = get<1>(order);
// 	price = get<2>(order);
// 	if (symbol != "----") {
// 		record_order(datetime,order);
// 		cash -= (num*price);
// 		holdings[symbol] += num;
// 	}
// }


// /* Executes a given list of orders */
// void Portfolio::listorder(string datetime, orderlist orders) {
// 	orderlist::iterator it;
// 	for (it = orders.begin(); it != orders.end(); ++it) {
// 		order(datetime, (*it));
// 	}
// }

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



