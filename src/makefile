CXX = g++
CXXFLAGS = -g -std=c++17

.PHONY: all client broker server clean test

all: client server broker

# Source files and object files
LIB_SRC = ./libs/message.cpp ./crdt/body/CausalHistories.cpp ./crdt/body/CLI.cpp ./crdt/body/Counter.cpp ./crdt/body/ShoppingList.cpp ./crdt/body/CounterMap.cpp
LIB_OBJ = $(LIB_SRC:.cpp=.o)

# Compile each library source file into an object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

client: ./client/client.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) ./client/client.cpp ./client/subscriber.cpp $(LIB_OBJ) -std=c++17 -o client.out -lzmq -lssl -lcrypto -lstdc++fs -Wno-deprecated-declarations

broker: ./server/broker.cpp ./server/consistentHashing.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) ./server/broker.cpp ./server/consistentHashing.cpp $(LIB_OBJ) -o broker.out -lzmq -lssl -lcrypto -Wno-deprecated-declarations

server: ./server/server.cpp ./server/handler.cpp ./server/md5.cpp ./server/consistentHashing.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) ./server/server.cpp ./server/handler.cpp ./server/md5.cpp ./server/consistentHashing.cpp $(LIB_OBJ) -o server.out -lzmq -lssl -lcrypto -Wno-deprecated-declarations

test:
	$(CXX) $(CXXFLAGS) ./test/test.cpp $(LIB_OBJ) -o test.out -lzmq -lssl -lcrypto -Wno-deprecated-declarations
	
clean:
	rm -f *.out $(LIB_OBJ)