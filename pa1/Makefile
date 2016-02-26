CXX=g++
CC=gcc
CFLAGS=-Wall
CXXFLAGS=-c -Wall -std=c++11
LDFLAGS=
CXXSOURCES=fakefs/main.cpp fakefs/filesystem.cpp
CXXOBJECTS=$(CXXSOURCES:.cpp=.o)
CXXEXECUTABLE=fakefs/fakefs
CEXECUTABLES=client/client server/server

all: $(CXXSOURCES) $(CXXEXECUTABLE) $(CEXECUTABLES)
	
# Compile fakefs
$(CXXEXECUTABLE): $(CXXOBJECTS)
	$(CXX) $(LDFLAGS) $(CXXOBJECTS) -o $@
.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

# Compile server and client
client/client: client/client.c
	$(CC) $(CFLAGS) $< -o $@
server/server: server/server.c
	$(CC) $(CFLAGS) $< -o $@

# Clean
clean:
	rm -rf $(CXXOBJECTS) $(CXXEXECUTABLE) $(CEXECUTABLES)
