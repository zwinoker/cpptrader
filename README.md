# cpptrader

This is a C++ framework for implementing and testing trading systems for the stock market. It supports up to minute-level resolution price data. Data is assumed to be stored in a MySQL datatbase.  


# Dependencies

* MySQL connector
* boost-date-time
* boost-system
* boost-filesystem
* libcurl

# Installation
Install the dependencies and then clone and compile cppTrader : 
```
git clone https://github.com/zwinoker/cpptrader.git
make
```
The executable is called ```run_simulation```.


# Data
Minute level data was scraped from Google Finance using the R script ```getPrices.r``` and stored in a MySQL database in the following format: Tables are named according to symbol (EX: "AAPL_", including the underscore). Each row in a table has the columns { Date-Time, Close, High, Low, Open, Volume } for each minute. Note that data is not included in this repository. Users must collect it themselves.

The symbols we collect data on and use are stored in symbolslist.csv. The current list is made up of symbols we can reliably get minute-level data on using the Google Finance API.


# Setting input parameters
Set parameters in ```simulation.in```. These are:
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

```live?``` is a boolean that is ```false``` if we're running a backtest, and ```true``` otherwise. ```tz_offset``` is your timezone offset (in hours) from New York City. ```max_ticks``` is the number of previous time steps our algorithm will consider at any given time. ```frequency``` is the number of time steps between each stored data point. ```tradefrequency``` is the number of time steps we wait between successive executions of the trading algorithm.

The date-times are in the format ```YYYY-MM-DD HH:MM:SS```, for example ```2015-03-05 15:09:00```.

# Usage
Modify ```algo.cpp``` to implement and execute your trading algorithm. The basic flow here is:
* Get relevant information from the ```Stock_Market``` object
* Use your trading system to generate one or more ```Order``` objects
* Give these to the ```Portfolio``` object using the ```order()``` method. These will get executed either immediately (if they are market orders) or at a certain price or date-time (if they are limit orders).

Once the input parameters are set and the trading system is implemented, run 
```
make
./run_simulation < simulation.in
```

# Useful methods
## Stock_Market.cpp
The stock_market object contains a few methods for retrieving price data. Use these for getting input data for your trading system.

## Order.cpp
Contains constructors for market and limit orders. Limit orders are also constructed using a stoploss price and a bool indicating if we want to buy/sell above or below that price.

## Portfolio.cpp
* ```order(Order)``` -- submit an Order.
* ```show_orderbook()``` -- prints your outstanding orders to the terminal
* ```show_portfolio()``` -- prints current portfolio holdings to the terminal

## Algo.cpp
* ```sell_all(Portfolio&, Stock_Market&)``` -- can be used to liquidate a portfolio at the current market value.
* ```eval(Portfolio&, Stock_Market&)``` -- called every ```frequency*tradefrequency``` minutes to execute trading algorithm.



# Results

The running value of the portfolio is written to ```portfolio.value```. The order history is written to ```portfolio.orderhistory```. Portoflio holdings are saved to ```portfolio.save```. 




