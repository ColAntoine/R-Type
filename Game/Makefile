# Simple Makefile for the Game UDP client

CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -I. -I../ECS/include
LDFLAGS = -pthread -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# ECS source files
ECS_SRCDIR = ../ECS/src
ECS_SRCS = $(ECS_SRCDIR)/registry.cpp $(ECS_SRCDIR)/systems.cpp $(ECS_SRCDIR)/components/library.cpp

SRCS = client.cpp game_window.cpp game_engine.cpp main.cpp
GAME_OBJS = $(SRCS:.cpp=.o)
ECS_OBJS = $(ECS_SRCS:.cpp=.o)
OBJS = $(GAME_OBJS) $(ECS_OBJS)

TARGET = r-type_client

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS) $(ECS_SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(ECS_SRCS) -o $(TARGET) $(LDFLAGS)
	@echo "Built $(TARGET)"

clean:
	rm -rf *.o $(TARGET)
	@echo "Cleaned"

run: all
	./$(TARGET)