# cpptrader

This is a C++ framework for implementing and testing trading systems for the stock market. It supports up to minute-level resolution price data. Data is assumed to be stored in a MySQL datatbase.  


# Install

The following libraries are required:
1. MySQL connector

2. Boost

3. curl

When these are installed, just navigate to the project directory and type ```make```. The executable is called ```run_simulation```.


# Data
Minute level data was scraped from Google Finance using the R script ```getPrices.r``` and stored in a MySQL database in the following format: Tables are named according to symbol (EX: "AAPL_", including the underscore). Each row in a table has the columns { Date-Time, Close, High, Low, Open, Volume } for each minute. 

The symbols we collect data on and use are stored in symbolslist.csv.


# Setting input parameters
After compiling the executable, set parameters in ```simulation.in```. These are:
```
file where we save portfolio holdings
start date-time
end date-time
live?
tz_offset
max_ticks
frequency
tradefrequency
MYSQL_SERVER
MYSQL_USERNAME
MYSQL_PASSWORD
MYSQL_DBNAME
```

```live?``` is a boolean that is ```false``` if we're running a backtest, and ```true``` otherwise. ```tz_offset``` is your timezone offset (in hours) from New York City. ````max_ticks``` is the number of previous time steps our algorithm will consider at any given time. ```frequency``` is the number of time steps between each stored data point. ```tradefrequency``` is the number of time steps we wait between successive executions of the trading algorithm.

# Usage
Modify ```algo.cpp``` to implement and execute your trading algorithm. The simulation interacts with the trading algorithm by calling ```algo.eval()```, so modify this method when implementing a trading system. Note that this method takes a reference to the Stock_Market and the Porftolio, so the latter can be modified directly by the algo.

Once the input parameters are set and the trading system is implemented, run 
```
make
./run_simulation < simulation.in
```

# Results

The running value of the portfolio is written to ```portfolio.value```. The order history is written to ```portfolio.orderhistory```. Portoflio holdings are saved to ```portfolio.save```. 




