# Compilacion del screensaver (pensado para Linux / WSL).
#
#   make                 compila build/screensaver
#   make run N=500       compila y ejecuta con 500 chuchos
#   make run N=2000 ARGS="--zona 100 --hilos 4"
#   make clean           borra la carpeta build/

CXX      := g++
# -fopenmp activa los #pragma omp (se usa al compilar y al enlazar)
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -fopenmp $(shell sdl2-config --cflags)
LDLIBS   := $(shell sdl2-config --libs)

SRC_DIR   := src
BUILD_DIR := build
TARGET    := $(BUILD_DIR)/screensaver

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

N    ?= 50
ARGS ?=

# Politica de espera de los hilos de OpenMP. Por defecto, al terminar una
# region paralela los hilos se quedan "girando" un rato; como en WSLg el
# dibujo tambien corre en la CPU (llvmpipe), eso le roba nucleos al render
# y baja los FPS. "passive" duerme a los hilos apenas terminan.
OMP_WAIT_POLICY ?= passive

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
	OMP_WAIT_POLICY=$(OMP_WAIT_POLICY) ./$(TARGET) $(N) $(ARGS)

clean:
	rm -rf $(BUILD_DIR)

-include $(OBJS:.o=.d)
