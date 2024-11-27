CXX = g++
CXXFLAGS = -g

.PHONY: all client server broker clean

all: client server broker

libs = ./libs/message.cpp ./crdt/body/CausalHistories.cpp ./crdt/body/CLI.cpp ./crdt/body/Counter.cpp ./crdt/body/ShoppingList.cpp

client:
	$(CXX) $(CXXFLAGS) ./client/client.cpp $(libs) -o client.out -lzmq

server:
	$(CXX) $(CXXFLAGS) ./server/server.cpp $(libs) -o server.out -lzmq

broker:
	$(CXX) $(CXXFLAGS) ./server/broker.cpp $(libs) -o broker.out -lzmq

clean:
	rm -f *.out