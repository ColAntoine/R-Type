# Makefile for the Game UDP client - supports both architectures

CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -I. -I../ECS/include -I../Network/include
LDFLAGS = -pthread -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# ECS source files
ECS_SRCDIR = ../ECS/src
ECS_SRCS = $(ECS_SRCDIR)/registry.cpp $(ECS_SRCDIR)/systems.cpp $(ECS_SRCDIR)/components/library.cpp

# Original client sources
SRCS = client.cpp game_window.cpp game_engine.cpp main.cpp
TARGET = r-type_client

# Service architecture client sources  
SERVICE_SRCS = client.cpp game_window.cpp main_service_arch.cpp
SERVICE_TARGET = r-type_client_service

.PHONY: all clean run service

all: $(TARGET)

service: $(SERVICE_TARGET)

$(TARGET): $(SRCS) $(ECS_SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(ECS_SRCS) -o $(TARGET) $(LDFLAGS)
	@echo "Built $(TARGET)"

$(SERVICE_TARGET): $(SERVICE_SRCS) $(ECS_SRCS)
	$(CXX) $(CXXFLAGS) $(SERVICE_SRCS) $(ECS_SRCS) -o $(SERVICE_TARGET) $(LDFLAGS)
	@echo "Built $(SERVICE_TARGET)"

clean:
	rm -rf *.o $(TARGET) $(SERVICE_TARGET)
	@echo "Cleaned"

run: all
	./$(TARGET)