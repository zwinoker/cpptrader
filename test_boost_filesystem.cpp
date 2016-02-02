

#include "Stock_Market.h"

#include <iostream>
#include <boost/filesystem.hpp>
//#include <boost/filesystem/fstream.hpp>
#include <fstream>
#include <sstream>

using namespace boost::filesystem;
using namespace std;

int main() {

	vector<string> symbolslist;
	string sym;
	ifstream symbols;
	symbols.open("symbolslist.csv");

	while(getline(symbols,sym,',')) {
		symbolslist.push_back(sym);

	}
	symbols.close();


	
	/* Name directory where we'll write the text data */
	path parentdirectory = current_path();
	path textdatafolder = current_path();
	textdatafolder /= "textdata";

	/* Create folder and navigate to it */
	if (exists(textdatafolder)) {
		remove_all(textdatafolder);
	}
	create_directory(textdatafolder);
	current_path(textdatafolder);

	/* For creating files */
	ofstream fs;

	vector<string>::const_iterator it;
	for (it = symbolslist.begin(); it != symbolslist.end(); ++it) {
		fs.open((*it));
		fs.close();
	}
	
	/* Reset current directory */
	current_path(parentdirectory);


	return 0;
}