CXX = g++
CXXFLAGS = -g

.PHONY: all client server broker clean

all: client server broker

client:
	$(CXX) $(CXXFLAGS) ./client/client.cpp ./libs/message.cpp -o clientt -lzmq

server:
	$(CXX) $(CXXFLAGS) ./server/server.cpp ./libs/message.cpp -o serverr -lzmq

broker:
	$(CXX) $(CXXFLAGS) ./server/broker.cpp ./libs/message.cpp -o broker -lzmq

clean:
	rm -f clientt serverr broker