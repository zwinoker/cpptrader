#File to print results to
resfile <- "simresults"
simulationinput <- read.csv('simulation.in', header=FALSE)

#Define simulation parameters
tradefreq <- 5
datafreq <- 1
pertradecommission <- 5
numoneminuteticks <- 25
initialcash <-100000
maxrisk <- 0.2

#Get the relevant data from our recorded portfolio values
portfoliores<-read.table('portfolio.value')
vals<-portfoliores[,3]
numvals<-length(vals)
valintervals = NULL
tradeintervals = NULL
for (i in 1:(numvals - 1)) {valintervals[i]=vals[i+1]-vals[i]}
for (i in seq(1, numvals - 1, tradefreq)) {tradeintervals[(i+4)/5]=vals[i+5]-vals[i]}
tradeintervals <- tradeintervals[which(!is.na(tradeintervals))]


#Calculate our analysis parameters
avgreturn <- mean(tradeintervals)

numgoodtrades <- length(which(tradeintervals > 0))
numbadtrades <- length(which(tradeintervals <= 0))
percentgoodtrades <- (numgoodtrades/(numgoodtrades + numbadtrades))

avgprofit <- sum(tradeintervals[which(tradeintervals > 0)]) / numgoodtrades
avgloss <- sum(tradeintervals[which(tradeintervals <= 0)]) / numbadtrades

commisionloss <- length(tradeintervals)*5


#Write results to file
write("Parameters ", resfile) 
write(paste("Trade frequency                        : ", tradefreq), resfile, append = TRUE)
write(paste("Data frequency                         : ", datafreq), resfile, append = TRUE)
write(paste("Per trade commision                    : ", pertradecommission), resfile, append = TRUE)
write(paste("Initial cash                     	    : ", initialcash), resfile, append = TRUE)
write(paste("Number of high resolution data points  : ", numoneminuteticks), resfile, append = TRUE)
write(paste("Max risk of capital (%)                : ", maxrisk), resfile, append = TRUE)

write("---------------------------------------------------", resfile, append = TRUE)
write("Results", resfile, append = TRUE)
write(paste("Total return (%)                       : ", 100*(vals[length(vals)] - initialcash)/initialcash), resfile, append = TRUE)
write(paste("Average return on trade                : ", avgreturn), resfile, append = TRUE)
write(paste("Average return of profitable trades    : ", avgprofit), resfile, append = TRUE)
write(paste("Average return of non-profitable trades: ", avgloss), resfile, append = TRUE)
write(paste("Percent of trades that were profitable : ", percentgoodtrades), resfile, append = TRUE)
write(paste("Money lost to commission               : ", commisionloss), resfile, append = TRUE)






