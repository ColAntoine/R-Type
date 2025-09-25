# Simple Makefile for the Game UDP client

CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -I.
LDFLAGS = -pthread

SRCS = client.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = r-type_client

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)
	@echo "Built $(TARGET)"

clean:
	rm -rf *.o $(TARGET)
	@echo "Cleaned"

run: all
	./$(TARGET)