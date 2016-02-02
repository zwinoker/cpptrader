#include "Stock_Market.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <memory>

#include <mysql.h>

using namespace std;

Stock_Market::Stock_Market() {

}

Stock_Market::Stock_Market(int frequency, int numticks, string startdate, 
						   const char* srvr, const char* uname, const char* pw, const char* dbase) {

	/* Convert SQL database params for the MYSQL_connect library to read */
	const char *server   = srvr;
	const char *username = uname;
	const char *password = pw;
	const char *database = dbase;

    /* Populate symbols list */
    string sym;
	ifstream symbols;
	symbols.open("symbolslist.csv");

	while(getline(symbols,sym,',')) {
		symbol_list.push_back(sym);
	}
	symbols.close();
    vector<string>::iterator it;

    MYSQL mysql;

	mysql_init(&mysql);
	mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"your_prog_name");
	if (!mysql_real_connect(&mysql,server,username,password,database,0,NULL,0))
	{
	    fprintf(stderr, "Failed to connect to database: Error: %s\n",
	          mysql_error(&mysql));
	}

	/* Open a connection to the MySQL database*/
	MYSQL_ROW row;

	unsigned int num_fields;
	unsigned int i;

	/* tick data */
	string thistime;
	vector<double> prices;
	tick_data ttd;
	vector<tick_data> pdata;

    /* Populate local market data table from the MySQL database. See Scraper::backtest_scrape() for more info */
    for (it = symbol_list.begin(); it != symbol_list.end(); ++it) {
		string s = "select * from (select * from " + ((*it) + "_") + " where Date < \'" + startdate + "\' and time(Date) < '16:00:00' and time(date) >= '09:30:00' order by Date desc limit " + to_string(frequency*numticks) + ") as bigdates where timestampdiff(minute, \'" + startdate + "\', Date) mod " + to_string(frequency) + " = 0 order by Date";

		mysql_query(&mysql, s.c_str());
        if(*mysql_error(&mysql))
		{
		  fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
		}

    	MYSQL_RES *result;
    	result = mysql_use_result(&mysql);

		num_fields = mysql_num_fields(result);
		
		while ((row = mysql_fetch_row(result)))
		{
		   unsigned long *lengths;
		   lengths = mysql_fetch_lengths(result);

		   thistime = row[0];
		   
		   for(i = 1; i < num_fields; i++){
		   		prices.insert(prices.end(), stod(string(row[i])));
		   }
		   ttd.first = thistime;
		   ttd.second = prices;
		   pdata.insert(pdata.begin(), ttd);
		   prices.clear();
		}
		marketdata[*it] = pdata;
		mysql_free_result(result);
		pdata.clear();
		
    }
    mysql_close(&mysql);
	
}

/* Version with fewer sql queries */
// Stock_Market::Stock_Market(int frequency, int numticks, int future_ticks, string startdate, 
// 						   const char* srvr, const char* uname, const char* pw, const char* dbase) {

// 	/* Wil help point to current tick datum to consider */
// 	current_elt = numticks - 1;
// 	earliest_elt = 0;

// 	/* Convert SQL database params for the MYSQL_connect library to read */
// 	const char *server   = srvr;
// 	const char *username = uname;
// 	const char *password = pw;
// 	const char *database = dbase;

//     /* Populate symbols list */
//     string sym;
// 	ifstream symbols;
// 	symbols.open("symbolslist.csv");

// 	while(getline(symbols,sym,',')) {
// 		symbol_list.push_back(sym);
// 	}
// 	symbols.close();
//     vector<string>::iterator it;

//     MYSQL mysql;

// 	mysql_init(&mysql);
// 	mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"your_prog_name");
// 	if (!mysql_real_connect(&mysql,server,username,password,database,0,NULL,0))
// 	{
// 	    fprintf(stderr, "Failed to connect to database: Error: %s\n",
// 	          mysql_error(&mysql));
// 	}

// 	/* Open a connection to the MySQL database*/
// 	MYSQL_ROW row;

// 	unsigned int num_fields;
// 	unsigned int i;

// 	/* tick data */
// 	string thistime;
// 	vector<double> prices;
// 	tick_data ttd;
// 	vector<tick_data> pdata;

//     /* Populate local market data table from the MySQL database. See Scraper::backtest_scrape() for more info */
//     for (it = symbol_list.begin(); it != symbol_list.end(); ++it) {
// 		//string s = "select * from (select * from " + ((*it) + "_") + " where Date < \'" + startdate + "\' and time(Date) < '16:00:00' and time(date) >= '09:30:00' order by Date desc limit " + to_string(frequency*numticks) + ") as bigdates where timestampdiff(minute, \'" + startdate + "\', Date) mod " + to_string(frequency) + " = 0 order by Date";
// 		string s = "select * from (select * from (select * from " + ((*it) + "_") + " where Date <= \'" + startdate + "\' and time(Date) < '16:00:00' and time(date) >= '09:30:00' order by date desc limit " + to_string(frequency*numticks) + ") as bigdates where timestampdiff(minute, \'" + startdate + "\', Date) mod " + to_string(frequency) + " = 0 order by Date) as t1 union select * from (select * from (select * from " + ((*it) + "_") + " where Date > \'" + startdate + "\' and time(Date) < '16:00:00' and time(date) >= '09:30:00' order by date limit " + to_string(frequency*future_ticks) + ") as bigdates where timestampdiff(minute, \'" + startdate + "\', date) mod " + to_string(frequency) + " = 0 order by date) as t2";


// 		mysql_query(&mysql, s.c_str());
//         if(*mysql_error(&mysql))
// 		{
// 		  fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
// 		}

//     	MYSQL_RES *result;
//     	result = mysql_use_result(&mysql);

// 		num_fields = mysql_num_fields(result);
		
// 		while ((row = mysql_fetch_row(result)))
// 		{
// 		   unsigned long *lengths;
// 		   lengths = mysql_fetch_lengths(result);

// 		   thistime = row[0];
		   
// 		   for(i = 1; i < num_fields; i++){
// 		   		prices.insert(prices.end(), stod(string(row[i])));
// 		   }
// 		   ttd.first = thistime;
// 		   ttd.second = prices;
// 		   pdata.insert(pdata.begin(), ttd);
// 		   prices.clear();
// 		}
// 		marketdata[*it] = pdata;
// 		mysql_free_result(result);
// 		pdata.clear();
		
//     }
//     mysql_close(&mysql);
	
// }




/* Returns a single tick datum (closing price, opening price, etc) */
double Stock_Market::get_info(string symbol, int info) {
	return (marketdata[symbol].front().second)[info];

}

//Version for faster backtests
/* Returns a single tick datum (closing price, opening price, etc) */
// double Stock_Market::get_info(string symbol, int info) {
// 	return (marketdata[symbol][current_elt].second)[info];

// }

/* Returns an entire vector of tick data for a single time */
vector<tick_data> Stock_Market::get_info_matrix(string symbol, int numsteps, int freq) {
	vector<tick_data> data_request;
	vector<tick_data>::iterator it = marketdata[symbol].begin();
	int counter = 0;
	int numelts = 0;
	int availablesteps = (marketdata[symbol]).size();
	while(counter < availablesteps && numelts < numsteps) {
		if (counter % freq == 0) {
			data_request.push_back(*it);
			numelts++;
		}
		counter++;
		++it;
	}
	return data_request;
}

/* Returns an entire matrix of tick data for a single time and multiple stocks */
vector<double> Stock_Market::get_info_vector(string symbol, int numsteps, int datatype, int freq) {
	vector<double> data_request;
	vector<tick_data>::iterator it = marketdata[symbol].begin();
	int counter = 0;
	int numelts = 0;
	int availablesteps = (marketdata[symbol]).size();
	double thisdatum = 0.0;
	while(counter < availablesteps && numelts < numsteps) {
		thisdatum = ((*it).second)[datatype];
		if (counter % freq == 0) {
			data_request.push_back(thisdatum);
			numelts++;
		}
		counter++;
		++it;
	}
	return data_request;
}

/* Checks that current date and last recorded date on a tick datum are the same */
bool Stock_Market::check_date(string symbol){
	return(marketdata[symbol].front().first == workingtime);
}

//Version for faster backtesting
/* Checks that current date and last recorded date on a tick datum are the same */
// bool Stock_Market::check_date(string symbol){
// 	return(marketdata[symbol][current_elt].first == workingtime);
// }

/* Prints stock market data to the console */
void Stock_Market::show() {
	cout<< "Current data: " << endl;

	market::iterator mi;
	for(mi = marketdata.begin(); mi != marketdata.end(); ++mi) {	

		vector<tick_data> pdata;
		vector<tick_data>::iterator it;

		vector<double> ttd;
		vector<double>::iterator ttdit;

		string datetime;

		cout<< mi->first << " : " <<endl;
		pdata = mi->second;
		
		for (it = pdata.begin(); it != pdata.end(); ++it) {
			ttd = (*it).second;
			datetime = (*it).first;
			cout << datetime << ", ";
			for (ttdit = ttd.begin(); ttdit != ttd.end(); ++ttdit){
				cout << *ttdit << ", ";
			}
			cout << endl;
		}
		cout << endl << endl;
	}

}



