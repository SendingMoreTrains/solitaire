# The compiler
CXX = g++

# Compiler flags
#  -g     - this flag adds debugging information to the executable
#  -Wall  - this flag is used to turn on most compiler warnings
CXXFLAGS = -g -Wall -I include/SDL2/

LDFLAGS = -L lib/
LDLIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

# SRC = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
# OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

SRC = src/main.cpp

all: $(OBJ)
	$(CXX) $(SRC) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -DBENCHMARK -o solitaire.exe
