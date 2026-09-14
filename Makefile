# Compilacion del screensaver y del benchmark (pensado para Linux / WSL).
#
#   make                 compila build/screensaver y build/benchmark
#   make run N=500       compila y ejecuta el screensaver con 500 chuchos
#   make run N=2000 ARGS="--zona 100 --hilos 4"
#   make bench           corre el benchmark con los valores por defecto
#   make bench BENCH_ARGS="--n 1000,2000 --hilos 1,4 --reps 3"
#   make clean           borra la carpeta build/

CXX      := g++
# -fopenmp activa los #pragma omp (se usa al compilar y al enlazar)
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -fopenmp $(shell sdl2-config --cflags)
LDLIBS   := $(shell sdl2-config --libs)

SRC_DIR   := src
BENCH_DIR := bench
BUILD_DIR := build

TARGET       := $(BUILD_DIR)/screensaver
BENCH_TARGET := $(BUILD_DIR)/benchmark

# Modulos compartidos: todo src/ menos el main del screensaver.
COMUN_SRCS := $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))
COMUN_OBJS := $(COMUN_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
MAIN_OBJ   := $(BUILD_DIR)/main.o

BENCH_SRCS := $(wildcard $(BENCH_DIR)/*.cpp)
BENCH_OBJS := $(BENCH_SRCS:$(BENCH_DIR)/%.cpp=$(BUILD_DIR)/$(BENCH_DIR)/%.o)

N          ?= 50
ARGS       ?=
BENCH_ARGS ?=

# Politica de espera de los hilos de OpenMP. Por defecto, al terminar una
# region paralela los hilos se quedan "girando" un rato; como en WSLg el
# dibujo tambien corre en la CPU (llvmpipe), eso le roba nucleos al render
# y baja los FPS. "passive" duerme a los hilos apenas terminan.
OMP_WAIT_POLICY ?= passive

.PHONY: all run bench clean

all: $(TARGET) $(BENCH_TARGET)

$(TARGET): $(COMUN_OBJS) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

$(BENCH_TARGET): $(COMUN_OBJS) $(BENCH_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

# -MMD -MP genera archivos .d para recompilar cuando cambia un .h
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/$(BENCH_DIR)/%.o: $(BENCH_DIR)/%.cpp | $(BUILD_DIR)/$(BENCH_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -MMD -MP -c $< -o $@

$(BUILD_DIR) $(BUILD_DIR)/$(BENCH_DIR):
	mkdir -p $@

run: $(TARGET)
	OMP_WAIT_POLICY=$(OMP_WAIT_POLICY) ./$(TARGET) $(N) $(ARGS)

bench: $(BENCH_TARGET)
	./$(BENCH_TARGET) $(BENCH_ARGS)

clean:
	rm -rf $(BUILD_DIR)

-include $(COMUN_OBJS:.o=.d) $(MAIN_OBJ:.o=.d) $(BENCH_OBJS:.o=.d)
