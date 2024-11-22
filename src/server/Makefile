# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include/openssl
LDFLAGS = -L/usr/lib -lssl -lcrypto

# Target executable
TARGET = main

# Source files
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)
	rm -f $(OBJS)

# Link the program
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
