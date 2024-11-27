CXX = g++
CXXFLAGS = -g

.PHONY: all client server broker clean

all: client server broker

client:
	$(CXX) $(CXXFLAGS) ./client/client.cpp ./libs/message.cpp -o client.out -lzmq

server:
	$(CXX) $(CXXFLAGS) ./server/server.cpp ./libs/message.cpp -o server.out -lzmq

broker:
	$(CXX) $(CXXFLAGS) ./server/broker.cpp ./libs/message.cpp -o broker.out -lzmq

clean:
	rm -f *.out