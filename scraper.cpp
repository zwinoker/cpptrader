#include "scraper.h"
#include <sstream>
#include <unistd.h>

#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <curl/curl.h>
#include <algorithm>
#include <array>

#include "boost_dt_extensions.h"

using namespace std;
using namespace boost::filesystem;
using namespace boost::posix_time;
using namespace boost::gregorian;

/* Default constructor */
Scraper::Scraper(){

}

/*pass the constructor algo.frequency and algo.max_ticks. Makes the url we'll want to scrape*/
Scraper::Scraper (int freq, int maxticks) {
	int i = 60*freq;

	max_ticks = maxticks;

	string istring = to_string(i);
	string mstring = to_string(freq);
	urlpart2 = "&i=" + istring + "&p=" + mstring + "m&f=d,o,c,h,l,v";

	/*Constructs the symbols list*/
	string sym;
	boost::filesystem::ifstream symbols;
	symbols.open("symbolslist.csv");

	int counter = 0;
	string symbolsrequest;

	while(getline(symbols,sym,',')) {
		symbolslist.push_back(sym);
		symbolsrequest += (sym + ",");
		if (!(counter < maxsymbols - 1)) {
			symbolsurl.push_back(symbolsrequest);
			symbolsrequest = "";
			counter = 0;
		}
		else {
			counter += 1;
		}
	}
	symbolsurl.push_back(symbolsrequest);
	symbols.close();

}

/* Constructor that takes in SQL params and data frequency info */
Scraper::Scraper (int freq, int maxticks, const char* servr, const char* usrname, const char* pword, const char* datbase) {
	/* Set SQL parameters */
	srvr = string(servr);
	uname = string(usrname);
	pw = string(pword);
	dbase = string(datbase);

	int i = 60*freq;
	max_ticks = maxticks;

	/* Build google finance URL */
	string istring = to_string(i);
	string mstring = to_string(freq);
	urlpart2 = "&i=" + istring + "&p=" + mstring + "m&f=d,o,c,h,l,v";

	/*Constructs the symbols list from a file*/
	string sym;
	boost::filesystem::ifstream symbols;
	symbols.open("symbolslist.csv");

	int counter = 0;
	string symbolsrequest;

	/* Set up symbols list and yahoo url request */
	while(getline(symbols,sym,',')) {
		symbolslist.push_back(sym);
		symbolsrequest += (sym + ",");
		if (!(counter < maxsymbols - 1)) {
			symbolsurl.push_back(symbolsrequest);
			symbolsrequest = "";
			counter = 0;
		}
		else {
			counter += 1;
		}
		
	}
	symbolsurl.push_back(symbolsrequest);
	symbols.close();

	/* Initialize the fileposition array */
	vector<string>::const_iterator it;
	for (it = symbolslist.begin(); it != symbolslist.end(); ++it) {
		filepos[(*it)] = 0;
	}

	initialize_sql_connection();

}

/* Opens SQL connection */
void Scraper::initialize_sql_connection() {
	const char *server   = srvr.c_str();
	const char *username = uname.c_str();
	const char *password = pw.c_str();
	const char *database = dbase.c_str();


	/* Open MySQL connection */
	mysql_init(&mysql);
	mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"your_prog_name");
	if (!mysql_real_connect(&mysql,server,username,password,database,0,NULL,0))
	{
	    fprintf(stderr, "Failed to connect to database: Error: %s\n",
	          mysql_error(&mysql));
	}


}

/* Closes sql connection */
void Scraper::cleanup() {
	/* Close MySQL connection */
    mysql_close(&mysql);

    /* Delete temp folder */
    path textdatafolder = current_path();
	textdatafolder /= "textdata";
	if (exists(textdatafolder)) {
		remove_all(textdatafolder);
	}
}

/* Create folder and text files for data storage */
void Scraper::create_text_files() {
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
	boost::filesystem::fstream fs;

	/* For each element in symbolslist, create textdatafolder/symbol */
	vector<string>::iterator it;
	for (it = symbolslist.begin(); it != symbolslist.end(); ++it) {
		fs.open((*it));
		fs.close();
	}
	
	/* Reset current directory */
	current_path(parentdirectory);
}

/* Gets full set of data for entire simulation and writes it to stock specific text files. Makes single line retrieval faster. */
void Scraper::initialize_text_data(string start, string end, int frequency) {

	/* Set up file system for text data*/
	create_text_files();
   
	/* MySQL variables */
	MYSQL_ROW row;
	unsigned int num_fields;
	unsigned int i;

	/* For iterating through symbols and associated data */
	vector<string>::iterator it;
	boost::filesystem::ofstream textdata;

	/* Change directory to the place where all text files are stored */
	path parentdirectory = current_path();
	path textdatafolder = current_path();
	textdatafolder /= "textdata";
	current_path(textdatafolder);

    /*Populate text data files*/
    for (it = symbolslist.begin(); it != symbolslist.end(); ++it) {

    	/* Retrieve data from SQL table */
    	string s = "select * from (select * from " + ((*it) + "_") + " where Date >= \'" + start + "\' and Date <= \'" + end + "\' and time(Date) < '16:00:00' and time(date) >= '09:30:00' order by Date) as bigdates where timestampdiff(minute, \'" + start + "\', Date) mod " + to_string(frequency) + " = 0 order by Date";
        mysql_query(&mysql, s.c_str());
        if(*mysql_error(&mysql))
		{
		  fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
		}
    	MYSQL_RES *result;
    	result = mysql_use_result(&mysql);
		num_fields = mysql_num_fields(result);

		/* Open file associated with this symbol */
		textdata.open((*it));
		
		/* Write each row to file */
		while ((row = mysql_fetch_row(result)))
		{
		   /* write to text file */
		   for(i = 0; i < num_fields; i++){
		   		textdata << row[i]; 
		   		if (i < num_fields - 1)
		   			textdata << ",";
		   }
		   textdata << endl; 
		}

		/* Clear results */
		textdata.close();
		mysql_free_result(result);
		
    }

    /* Reset current directory */
	current_path(parentdirectory);
}

/* Gets data from text files */
void Scraper::text_scrape(market &mdata, string currentdatetime) {
	/* Change directory to the place where all text files are stored */
	path parentdirectory = current_path();
	path textdatafolder = current_path();
	textdatafolder /= "textdata";
	current_path(textdatafolder);

	boost::filesystem::ifstream textdata;
	ptime currenttime = time_from_string(currentdatetime);
	ptime filetime;

	/* For updating each stock's data set */
	string thistime;
	vector<double> prices;
	tick_data ttd;
	vector<tick_data> pdata;
	vector<string>::const_iterator it;

	/* For parsing csv format. */
	string line, dummyline;
	stringstream ss;

	/* Attempt to get most recent tick data for  */
	for (it = symbolslist.begin(); it != symbolslist.end(); ++it) {

		/* Open file associated with this symbol and read the date in the most recent line */
		textdata.open((*it));

		textdata.seekg(filepos[(*it)]);

		getline(textdata, line, ',');

		/* Checks if the file is empty or not. If empty, just go to next file */
		if (!line.empty()) {
			filetime = time_from_string(line);
			
			/* While first line is date less than current date --> discard first line and repeat on second line */
			while(filetime < currenttime) {

				/* Skip to next line  */
				getline(textdata, line);
				getline(textdata, line, ',');
				if (line.empty())
					break;
				/* Update datetime from top row */
				filetime = time_from_string(line);
			}
			/* First line is current date --> scrape it. Else do nothing*/
			if (filetime == currenttime) {

				/* Stock's current data set */
				pdata = mdata[*it];

				/* Make space for the new datum */
				if (pdata.size() >= max_ticks)
	      			pdata.pop_back();

	      		/* Get datetime from this row */
	      		ttd.first = line;

	      		/* Get price data from this row */
	      		getline(textdata, line);

	      		ss << line;

	      		while(getline(ss, dummyline, ',')) {
	      			prices.insert(prices.end(), stod(dummyline));
	      		}

	      		ttd.second = prices;

		   		pdata.insert(pdata.begin(), ttd);
		   		prices.clear();
		   		mdata[*it] = pdata;

		   		/* Cleanup */
		   		ss.clear();

		   		//Store current streampos . 
				filepos[(*it)] = textdata.tellg();
	      		
			}

		}

		textdata.close();
    }

    /* Reset current directory */
	current_path(parentdirectory);
}

/* Gets data from SQL database. */
void Scraper::backtest_scrape(market &mdata, string datetime){

    /*Populate symbols list*/
    vector<string> symbol_list;
    string sym;
	boost::filesystem::ifstream symbols;
	symbols.open("symbolslist.csv");

	while(getline(symbols,sym,',')) {
		symbol_list.push_back(sym);
	}
	symbols.close();
    vector<string>::iterator it;

	MYSQL_ROW row;

	unsigned int num_fields;
	unsigned int i;

	/* tick data info */
	string thistime;
	vector<double> prices;
	tick_data ttd;
	vector<tick_data> pdata;

    /*Populate local market data table*/
    for (it = symbol_list.begin(); it != symbol_list.end(); ++it) {

		string s = "select * from " + ((*it) + "_") + " where Date = \'" + datetime + "\'";
        mysql_query(&mysql, s.c_str());

        if(*mysql_error(&mysql))
		{
		  fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
		}

    	MYSQL_RES *result;
    	result = mysql_use_result(&mysql);

		num_fields = mysql_num_fields(result);

		pdata = mdata[*it];
		
		while ((row = mysql_fetch_row(result)))
		{

		   if (pdata.size() >= max_ticks)
	      		pdata.pop_back();
		   unsigned long *lengths;
		   lengths = mysql_fetch_lengths(result);
		   
		   for(i = 1; i < num_fields; i++){
		   		prices.insert(prices.end(), stod(string(row[i])));
		   }
		   ttd.first = datetime;
		   ttd.second = prices;
		   pdata.insert(pdata.begin(), ttd);
		   prices.clear();
		}

        /* Update stock_market's data */
		mdata[*it] = pdata;

		/* Clear MySQL results */
		mysql_free_result(result);
		pdata.clear();
		
	
    }
} 



/* Callback function for curl that parses the data from the google finance scrape */
static size_t get_data(char *buffer, size_t size, size_t nmemb, void *pdata) {
	double *newpdata = (double *) pdata;
	stringstream ss;
    ss << buffer;
	string line;
	double d;
	size_t* sz = 0;
	while (getline(ss, line)){
		if(line[0] == 'a'){//read to line right before the tick data starts
			getline(ss,line,','); //skip first string before first comma
			for (int i = 0; i < 4 ; i++){
				getline(ss,line,',');
				d = stod(line, sz);
				newpdata[i] = d;
			}
			getline(ss, line,'\n'); //fill in data for volume, which has a line break after it
			d = stod(line, sz);
			newpdata[4] = d;
		}
	}
	memcpy(&pdata,&newpdata,sizeof(newpdata));
 	return size*nmemb;
};

/* Gets data from google finance */
void Scraper::scrape(market &mdata, string datetime){
	double pdataholder[5];

	vector<string>::iterator it;
	CURL *curl;
  	CURLcode cres;
  	string url;
 
  	curl = curl_easy_init();
  	tick_data info;

  	if(curl) {
  		/*loop through symbols list here and retrieve info from google finance*/
  		for (it = symbolslist.begin(); it != symbolslist.end(); ++it) {

			url = urlpart1 + (*it) + urlpart2;
		    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)pdataholder);
		    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_data);
		    cres = curl_easy_perform(curl);

		 	/* Check for errors */ 
	    	if(cres != CURLE_OK)
	      		fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(cres));

	 		info.second.clear();
	      	for (int j = 4 ; j > -1 ; j--) 
	      		info.second.insert(info.second.begin(),pdataholder[j]);
	 		/*Add data to market*/
	      	info.first = datetime;
	      	if (mdata[*it].size() >= max_ticks)
	      		mdata[*it].pop_back();
	      	mdata[*it].insert(mdata[*it].begin(),info);
		}
		/* always cleanup */ 
		curl_easy_cleanup(curl);	
  	}
} 

/*Callback function for curl that parses the data from the yahoo finance scrape*/
static size_t yahoo_get_data(char *buffer, size_t size, size_t nmemb, void *pdata) {
	double *newpdata = (double *) pdata;
	stringstream ss, eachline;
    ss << buffer;
	string line, datum;
	double d;
	size_t* sz = 0;
	int symbolcounter = 0;
	int datacounter = 0;
	int urlcalls = newpdata[0];
	int symbolindex;
	while (getline(ss, line)){
		eachline << line;
			while(getline(eachline, datum, ',')) {
				if (symbolcounter == Scraper::maxsymbols){
				 	break;
				}
				else {
					try {
						d = stod(datum, sz);
					}
					catch (const std::invalid_argument& ia) {
						d = 0.00;
					}
						symbolindex = 4 + (symbolcounter * Scraper::numdatatypes) + (urlcalls * (Scraper::maxsymbols * Scraper::numdatatypes));
					if (datacounter % Scraper::numdatatypes == 3) {
						symbolcounter++;
					}

					*(newpdata+symbolindex+datacounter) = d;
					datacounter = (datacounter + 1) % Scraper::numdatatypes;
					
				}
			}
		eachline.clear();
		eachline.str(std::string());
	}
	memcpy(&pdata,&newpdata,sizeof(newpdata));
 	return size*nmemb;
};


/* Gets data from yahoo finance. Here, the data is stored as [close, ask, bid, running volume] */
void Scraper::yahoo_scrape(market &mdata, string datetime) {

	int numsymbols = symbolslist.size();
	int numgetdatacalls = 0;
	
	double pdataholder[(numsymbols * numdatatypes) + 4]; //first element keeps track of which url call we're on
  	pdataholder[0] = 0.0;

	vector<string>::iterator it;
	CURL *curl;
  	CURLcode cres;
  	string url;
 
  	curl = curl_easy_init();
  	tick_data info;

  	if(curl) {
  		/*loop through symbols list here and retrieve info from yahoo finance*/
  			vector<string>::iterator it;

  			for (it = symbolsurl.begin(); it != symbolsurl.end(); ++it) {
	  			url = "http://download.finance.yahoo.com/d/quotes.csv?s=%40%5EDJI," + (*it) + "&f=l1b2b3v0&e=.csv";

			    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)pdataholder);
			    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, yahoo_get_data);
			    cres = curl_easy_perform(curl);

			 	/* Check for errors */ 
		    	if(cres != CURLE_OK)
		      		fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(cres));
		      	numgetdatacalls++;
		      	pdataholder[0] = (double) numgetdatacalls;
	      	}

	 		/*Add data to market*/
	      	info.first = datetime;
	      	int counter = 0; //start at second element of pdataholder, since first is the number of times we've scraped
	      	for (it = symbolslist.begin(); it!= symbolslist.end(); ++it) {
	      		info.second.clear();
	      		for (int j = 3 ; j > -1 ; j--)
	      			info.second.insert(info.second.begin(),pdataholder[4 + (counter*numdatatypes) + j]);
	      		if (mdata[*it].size() >= max_ticks)
	      			mdata[*it].pop_back();
	      		mdata[*it].insert(mdata[*it].begin(),info);
	      		counter++;
	      	}
	      	
		}
		curl_easy_cleanup(curl);	
  	}





