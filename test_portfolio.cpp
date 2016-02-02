#include "portfolio.h"

#include <iostream>

using namespace std;

int main() {
	
	Portfolio p("portfolio.save");
	p.show_portfolio();
	p.save();

	vector<pair<string,int> > testassetlist;
	pair<string, int> p1("AAPL", 4);
	pair<string, int> p2("INTL", 5);
	pair<string, int> p3("DELL", 1);
	testassetlist.push_back(p1);
	testassetlist.push_back(p2);
	testassetlist.push_back(p3);
	Portfolio a(102.12, testassetlist);
	a.show_portfolio();

	return 0;
}