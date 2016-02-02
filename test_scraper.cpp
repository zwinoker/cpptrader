#include "scraper.h"

#include <iostream>

#include "Stock_Market.h"

using namespace std;

int main() {
	
	

	int frequency = 1, maxticks = 5;
	string s1 = "localhost", s2 = "root", s3 = "sqlpass", s4 = "stockmarket";
	string start = "2015-03-10 10:00:00", end = "2015-03-10 11:00:00";

	Scraper s(frequency,maxticks, s1.c_str(), s2.c_str(), s3.c_str(), s4.c_str());

	s.initialize_text_data(start, end, frequency);
	

	market tmark;

	s.text_scrape(tmark, "2015-03-10 11:00:00");

	market::iterator it;
	vector<double>::iterator vit;
	vector<tick_data> ttdvector;
	vector<tick_data>::iterator ttit;
	tick_data info;
	vector<double> pdata;

	for(it = tmark.begin(); it != tmark.end(); ++it) {
			cout << it->first << " : " << endl;;
			ttdvector = it->second;
			if (!ttdvector.empty()) {
				for (ttit = ttdvector.begin() ; ttit != ttdvector.end(); ++ttit) {
					info = (*ttit);
					cout << info.first << ", ";
					pdata = info.second;
					for (vit = pdata.begin(); vit != pdata.end(); ++vit)
						cout << *vit << ", ";
					cout << "\n\n";
				}
			}
	}

	//s.cleanup();


	// pdata.clear();
	// ttdvector.clear();

	// Stock_Market st(frequency,maxticks,"2014-07-14 09:53:00");
	// st.show();


	// s.backtest_scrape(st.marketdata,"2014-07-14 09:54:00");
	// s.backtest_scrape(st.marketdata,"2014-07-14 09:55:00");
	// s.backtest_scrape(st.marketdata,"2014-07-14 09:56:00");

	// st.show();


	// for(it = backtest.begin(); it != backtest.end(); ++it) {
	// 	cout << it->first << " : ";
	// 	ttdvector = it->second;
	// 	info = ttdvector.front();
	// 	cout << info.first << ", ";
	// 	pdata = info.second;
	// 	for (vit = pdata.begin(); vit != pdata.end(); ++vit)
	// 		cout << *vit << ", ";
	// 	cout << '\n';
	// }



	return 0;
}