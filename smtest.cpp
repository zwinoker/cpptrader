//compile like this
//clang++ -std=c++11 -o smtest smtest.cpp Stock_Market.cpp -L/usr/local/opt/mysql-connector-c++/lib -I/usr/local/mysql/include -lmysqlcppconn-static  -L/usr/local/lib -I/usr/local/opt/mysql-connector-c/include -lmysqlclient

#include "Stock_Market.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

#include <mysql.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

using namespace std;

int main() {
	vector<string> symlist;
	symlist.push_back("AAPL");

	symlist.push_back("NFLX");

	Stock_Market sm (symlist, "2014-05-19", "2014-07-22");
	sm.show();

	cout << sm.get_info("NFLX", "2014-07-21 10:00:00", 0) << endl;

	return 0;
}