/* The scraper object handles data collection from either an SQL database or yahoo finance. 
	It then updates the stock_market object accordingly. */

#include <string>
#include <map>
#include <vector>
#include <mysql.h>

#include <boost/filesystem.hpp>

#include "types.h"
#include <fstream>


class Scraper {
	public:
		/* Constructors */
		Scraper(); /* Default  */
		Scraper (int, int); /* Takes frequency and max_ticks from algo */
		Scraper (int, int, const char*, const char*, const char*, const char*); /* Also takes SQL database connection info */
	
		/* Strings for google finance data collection */
		std::string urlpart1 = "http://www.google.com/finance/getprices?q=";
		std::string urlpart2;
		std::vector<std::string> symbolsurl;

		/* Parameters for efficient data collection from yahoo finance */
		static const int maxsymbols = 30;
		static const int numdatatypes = 4;

		/* SQL connection parameters */
		std::string srvr, uname, pw, dbase;
		MYSQL mysql;

		/* Max number of data points to collect */
		int max_ticks;

		/* For general collection and pricedata updating */
		std::vector<std::string> symbolslist;
		std::vector<double> pricedata; 
		
		/* Data Collection methods */
		void text_scrape(market&, std::string); /* Gets data from text files. String is the current time. */
		void backtest_scrape(market&, std::string); /* Gets data from sql table instead of from live data source. Used for backtesting.*/
		void scrape(market&, std::string); /* Scrapes live data from google finance */
		void yahoo_scrape(market&, std::string); /* Scrapes live data from yahoo finance */

		/* Close sql connection, delete temp files. */
		void cleanup();

		/* Creates text data files for the current simulation. Takes in a start and end time, frequency,  */
		void initialize_text_data(std::string, std::string, int);

		
	private:
		/* Create the MySQL connection */
		void initialize_sql_connection();

		/* Map from symbol name to current streampos in their text files */
		std::map<std::string, std::streampos> filepos;

		/* Folder name where we store text files with data */
		void create_text_files();
		// boost::filesystem::path workingdirectory;
		// boost::filesystem::path textdatafolder; 
		
		



};









