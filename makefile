CC = clang++
CFLAGS = -std=c++11 -stdlib=libc++ -o 
LIBS = `mysql_config --cflags --libs` -lcurl -lboost_date_time -lfftw3 -lboost_system -lboost_filesystem -L/usr/local/lib
INCLUDES = -I/usr/local/include/
SOURCES = test_simulation.cpp simulation.cpp scraper.cpp Stock_Market.cpp portfolio.cpp algo.cpp order.cpp 
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = test_simulation

all: $(EXECUTABLE)

$(EXECUTABLE): 
	$(CC) $(CFLAGS) $@ $(SOURCES) $(LIBS) $(INCLUDES)

clean:
	rm $(EXECUTABLE)