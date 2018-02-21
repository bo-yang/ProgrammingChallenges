CC=g++
CFLAGS=-c -g -Wall -std=c++11 -I/usr/include/mysql -I/usr/local/include/mysql++
LDFLAGS=-lldns -lmysqlpp #-lmysqlclient
SOURCES=dns_stats.cc main.cc
OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=dns_stats

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
