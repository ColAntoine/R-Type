# Simple Makefile for the Network UDP server

CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -I.
LDFLAGS = -pthread

SRCS = server.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = r-type_server

.PHONY: all clean run

all: $(TARGET)


$(TARGET): $(BINDIR) $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)
	@echo "Built $(TARGET)"

clean:
	rm -rf $(OBJS) $(TARGET)
	@echo "Cleaned"

run: all
	./$(TARGET)
