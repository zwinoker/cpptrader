#include "simulation.h"
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>

#include <mysql.h>

using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

/* DEFAULT CONSTRUCTOR:

	Reads from input file of form:
		portfoliosavefile
		startdate
		enddate
		live boolean (0 or 1)
		tz_offset

		//Algo parameters\\
		max_ticks
		frequency
		tradefrequency

		//SQL parameters\\
		server
		username
		password
		database name

 */
Simulation::Simulation(){
	int max_ticks, frequency, tradefrequency;
	double transactioncost, maxriskablecapital, maxdrawdown; 
	
	/* Read simulation parameters from input file*/
	char tester[256];
	cin.getline(tester,256);
	portfoliosavefile = string(tester);
	cin.getline(tester,256);
	startdate = string(tester);
	cin.getline(tester,256);
	enddate = string(tester);
	cin.getline(tester,256);
	live = (string(tester) == "true");
	cin.getline(tester,256);
	tz_offset = max_ticks = stoi(string(tester));

	/* Read algo parameters from input file*/
	cin.getline(tester,256);
	max_ticks = stoi(string(tester));
	cin.getline(tester,256);
	frequency = stoi(string(tester));
	cin.getline(tester,256);
	tradefrequency = stoi(string(tester));

	/* Read SQL parameters from input file*/
	cin.getline(tester,256);
	const char *server = string(tester).c_str();
	cin.getline(tester,256);
	const char *username = string(tester).c_str();
	cin.getline(tester,256);
	const char *password = string(tester).c_str();
	cin.getline(tester,256);
	const char *database = string(tester).c_str();

	/* Changes start date to the current date-time if we are doing a live test. */
	if (live) {
		startdate = date_time_to_string(second_clock::local_time() + hours(3));
	}

	p = Portfolio(portfoliosavefile);
	
	trader = Algo(max_ticks, frequency, tradefrequency);

	sm = Stock_Market(trader.frequency,trader.max_ticks, startdate, server, username, password, database);

	scr = Scraper(trader.frequency,trader.max_ticks, server, username, password, database);

} 

/* Writes headers for portfolio.value csv file */ 
void write_portfolio_value_headers() {
	ofstream writefile("portfolio.value", std::fstream::trunc);
	string info = "date,close\n";
	writefile << info;
	writefile.close();
}

/* Records the portfolio's current value to a file. */
void record_portfolio_value(double val, string datetime) {
	fstream writefile("portfolio.value", std::fstream::app);
	string info = datetime + ", " + to_string(val) + "\n";
	writefile << info;
	writefile.close();
}

/* Determines the total value of all assets held in the portfolio. */
double Simulation::value_portfolio(string datetime) {
	double value = p.cash;
	map<string,int>::iterator it;
	int num = 0;
	if (p.holdings.size()!= 0){
		for (it = p.holdings.begin(); it!= p.holdings.end() ; ++it) {
			num = it->second;
			value += (num*(sm.get_info(it->first,0)));
		}
	}
	return value;
}

/* Records orders*/
void record_orders(string datetime, orderlist orders) {
	fstream writefile("portfolio.orderhistory", std::fstream::app);
	orderlist::iterator it;
	string info, symbol;
	int num;
	double price;
	for (it = orders.begin(); it != orders.end(); ++it) {
		num = get<1>(*it);
		if (num != 0) {
			symbol = get<0>(*it);
			price = get<2>(*it);
			string info = datetime + ", " + symbol + ", " + to_string(num) + ", " + to_string(price) + "\n";
			writefile << info;
		}
		
	}
	writefile.close();
}


/* Begins the backtest or live test. (Just backtest for now, live test isn't working yet) */
void Simulation::run_simulation() {

	ptime pstart(time_from_string(startdate));
	
	write_portfolio_value_headers();

	orderlist orders;

	/* Live run. */
	if (live) {
		ptime currenttime = second_clock::local_time() + hours(tz_offset);
		pstart = currenttime;
		ptime dayend(pstart.date(), hours(20)); //same day but at 4PM

		if (is_weekend(pstart) || pstart >= dayend) {
			cout << "Cannot live test because the market isn't open." << endl;
		}
		else {
			
			cout << "Beginning live test." << endl;
			

			ptime lasttrade = pstart;
			/* This loop advances the simulation through the day. */
			while(pstart < dayend) {

				/* If end of day, liquidate. */
				if (pstart + minutes(trader.tradefrequency) >= dayend) {
					trader.sell_all(p,sm);
					break; /* Done trading for the day, so stop the while loop */ 
				}


				/* Otherwise, see if we want to trade. */
				else {

					/* Else update stock_market's locally stored information. */
					sm.workingtime = date_time_to_string(pstart);
					cout << "\n\n\nEvaluating at: " << sm.workingtime<< endl;
					scr.yahoo_scrape(sm.marketdata,sm.workingtime);

					/* See if there are any outstanding orders that need to be executed */
					trader.execute_order_book(sm, p);

					/* Run the algo if tradefrequency minutes have passed */
					if (ptime_mod((pstart - lasttrade), minutes(trader.tradefrequency)) == minutes(0)) {
						lasttrade = pstart;
						trader.eval(sm,p);
					}
				}

				record_portfolio_value(value_portfolio(sm.workingtime),sm.workingtime);

				pstart += minutes(trader.frequency);
				while(currenttime < pstart) {
					currenttime = second_clock::local_time() + hours(tz_offset); 
				}

				p.save();
			}

		}
	}



	/* Back testing */
	else {

		//Tell the scraper to initialize the backtesting text data here IMPLEMENT THIS
		scr.initialize_text_data(startdate, enddate, trader.frequency);

		cout << "Beginning historical data test." << endl;
		ptime pend(time_from_string(enddate));
		ptime lasttrade;

		/* The outer while loop continues the simulation until the current time equals the end time. */
		while(pstart < pend) {

			/* This loop skips over weekends, when the market isn't open. */
			while(is_weekend(pstart)) {
				pstart += days(1);
				if (pstart >= pend) break;
			}

			/* Assigns daily start and end times. Lets us skip to next day once the market closes. */
			ptime dayend(pstart.date(), hours(16)); //same day but at 4PM
			ptime daystart(pstart.date(), hours(9) + minutes(31));
			lasttrade = daystart;

			/* This loop advances the simulation through the day. */
			while(pstart < dayend && pstart <= pend) {
				sm.workingtime = date_time_to_string(pstart);
				cout << "\n\n\nEvaluating at : " << sm.workingtime << endl;


				/* If end of day, liquidate */
				if (pstart + minutes(trader.frequency) >= dayend) {
					trader.sell_all(p,sm);
				}

				/* Otherwise, see if we want to trade. */
				else {
					/* Update stock_market's locally stored information. */
					scr.text_scrape(sm.marketdata,sm.workingtime);

					/* See if there are any outstanding orders that need to be executed */
					trader.execute_order_book(sm, p);

					if (ptime_mod((pstart - lasttrade), minutes(trader.tradefrequency)) == minutes(0)) {
						lasttrade = pstart;
						trader.eval(sm,p);
					}
				}

				/* Save the portfolio's value to a file. */
				record_portfolio_value(value_portfolio(sm.workingtime),sm.workingtime);

				/* Advance the time and save the portfolio's contents. */
				pstart += minutes(trader.frequency);
				p.save();
				
			}
			/* Advance time to next day at market open */
			pstart = ptime(pstart.date() + days(1),hours(9) + minutes(31)); 
		}
		scr.cleanup();
	}

	cout << "\n\nSimulation over.\n\n" << endl;
	
}


