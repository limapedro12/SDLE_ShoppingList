CXX = g++
CXXFLAGS = -g

.PHONY: all client server broker clean crdt

all: client server broker

libs = ./libs/message.cpp ./crdt/body/CausalHistories.cpp ./crdt/body/CLI.cpp ./crdt/body/Counter.cpp ./crdt/body/ShoppingList.cpp

client:
	$(CXX) $(CXXFLAGS) ./client/client.cpp $(libs) -o client.out -lzmq

server:
	$(CXX) $(CXXFLAGS) ./server/server.cpp $(libs) ./server/handler.cpp ./server/md5.cpp -o server.out -lzmq -lssl -lcrypto

broker:
	$(CXX) $(CXXFLAGS) ./server/broker.cpp $(libs) -o broker.out -lzmq

crdt:
	$(CXX) $(CXXFLAGS) ./crdt/body/CausalHistories.cpp ./crdt/body/CLI.cpp ./crdt/body/Counter.cpp ./crdt/body/ShoppingList.cpp -o crdt.out -lzmq

clean:
	rm -f *.out