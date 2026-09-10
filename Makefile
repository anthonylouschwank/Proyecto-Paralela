# Compilacion del screensaver (pensado para Linux / WSL).
#
#   make                 compila build/screensaver
#   make run N=500       compila y ejecuta con 500 chuchos
#   make clean           borra la carpeta build/

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra $(shell sdl2-config --cflags)
LDLIBS   := $(shell sdl2-config --libs)

SRC_DIR   := src
BUILD_DIR := build
TARGET    := $(BUILD_DIR)/screensaver

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

N ?= 50

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

# -MMD -MP genera archivos .d para recompilar cuando cambia un .h
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET) $(N)

clean:
	rm -rf $(BUILD_DIR)

-include $(OBJS:.o=.d)
