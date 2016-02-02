//compile like this
//clang++ -std=c++11 -o run_sim run_sim.cpp simulation.cpp Stock_Market.cpp portfolio.cpp -L/usr/lib/i386-linux-gnu -lmysqlclient -lboost_date_time -I/usr/include/cppconn -I/usr/include/boost/date_time -I/usr/include/mysql -L/usr/lib -lmysqlcppconn-static 
//args should be of form:
// startingcash	 startdate	 enddate  algopath	symbol1	 shares1  symbol2  shares2 .....

#include "simulation.h"
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <sstream>

using namespace std;


int main(int argc, char* argv[]) {
	vector<pair<string,int> > testassetlist;
	stringstream converter;

	//remember: argv[0] is the program name
	for (int i = 5; i < argc; i=i+2) {
		converter << argv[i];

		//atoi is the converter to int
		testassetlist.push_back(pair<string,int>(converter.str(),atoi(argv[i+1])));
		converter.str("");
	}
	
	int money;
	string start;
	string end;
	string alg_file;

	money = atoi(argv[1]);

	converter << argv[2];
	start = converter.str();
	start += " 09:31:00";
	converter.str("");

	converter << argv[3];
	end = converter.str();
	end += " 16:00:00";
	converter.str("");

	converter << argv[4];
	alg_file = converter.str();
	converter.str("");



	//Simulation s(1000.0, testassetlist, "2014-05-19 09:31:00", "2014-07-21 16:00:00", "test");
	Simulation s(money, testassetlist, start, end, alg_file);
	s.run_simulation();


	return 0;

}

//runscript: ./run_sim 1000.0 2014-05-19 2014-07-21 test AAPL 5 NFLX 1 INTC 3




