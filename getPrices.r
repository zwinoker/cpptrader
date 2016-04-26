#For scraping finance data from Google Finance and storing in MySQL database

# Config variables. Set these before running the script
workingDir = ""
MYSQL_USER = ""
MYSQL_PASSWORD = ""
MYSQL_DBNAME = ""

#library for retrieving data from a URL
library("RCurl")
library("RMySQL")
setwd(workingDir)
source("csvreader.r")


#List of symbols for NASDAQ100 and S+P500
symbols<- read.table("/home/zack/trading/scraper/symbolslist.csv", header = FALSE, sep = ",")

#Collects data the day after the market closes, so we subtract by one day
thedate<- toString(Sys.Date() - 1) 
sqlcon<-dbConnect(MySQL(), user=MYSQL_USER, password=MYSQL_PASSWORD, dbname=MYSQL_DBNAME)


#Gets yesterdays minute level data on symbol x and stores it in the MySQL database "stockmarket"
getnstore<- function(x) {
	print(x)
	
	#Writes scraped data to temporary file
	pURL<- paste("http://www.google.com/finance/getprices?q=", x, "&i=60&p=1d&f=d,o,c,h,l,v", sep = "")
	pinfo<- getURL(pURL) 
	tempfile<-"temp.csv"
	data<-file(tempfile)
	writeLines(pinfo, data)
	close(data)

	#Cleans scraped data
	bla<-readin(thedate,tempfile)

	#Adds scraped data to SQL database
	dbn<-paste(x,"_",sep='')
	if (length(bla$Date)!=0) {
		#If the table exists, we just add to it instead of making a new one
		addtotable <- dbExistsTable(sqlcon, dbn)
		dbWriteTable(sqlcon,dbn,bla,row.names=FALSE,append=addtotable)
	}

	#Deletes temp file
	file.remove(tempfile)

}

#Run the scraper
apply(symbols, 2, function(x) getnstore(x))

dbDisconnect(sqlcon)




