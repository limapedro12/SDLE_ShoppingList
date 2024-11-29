CXX = g++
CXXFLAGS = -g

.PHONY: all client server broker clean

all: client server broker

# Source files and object files
LIB_SRC = ./libs/message.cpp ./crdt/body/CausalHistories.cpp ./crdt/body/CLI.cpp ./crdt/body/Counter.cpp ./crdt/body/ShoppingList.cpp
LIB_OBJ = $(LIB_SRC:.cpp=.o)

# Compile each library source file into an object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

client: ./client/client.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) ./client/client.cpp $(LIB_OBJ) -o client.out -lzmq

server: ./server/server.cpp ./server/handler.cpp ./server/md5.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) ./server/server.cpp ./server/handler.cpp ./server/md5.cpp $(LIB_OBJ) -o server.out -lzmq -lssl -lcrypto

broker: ./server/broker.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) ./server/broker.cpp $(LIB_OBJ) -o broker.out -lzmq

clean:
	rm -f *.out $(LIB_OBJ)