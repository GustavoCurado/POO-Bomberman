CC = g++
CFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
INCLUDE = -I ./src/include
SRC = ./src
OBJ = ./obj
BIN = ./bin

all: create_obj $(wildcard *.cpp)
	$(CC) -c $(SRC)/map.cpp $(INCLUDE) $(CFLAGS) -o $(OBJ)/map.o
	$(CC) -c $(SRC)/game.cpp $(INCLUDE) $(CFLAGS) -o $(OBJ)/game.o
	$(CC) -c $(SRC)/entity.cpp $(INCLUDE) $(CFLAGS) -o $(OBJ)/entity.o
	$(CC) -c $(SRC)/bomberman.cpp $(INCLUDE) $(CFLAGS) -o $(OBJ)/bomberman.o
	$(CC) -c $(SRC)/enemy.cpp $(INCLUDE) $(CFLAGS) -o $(OBJ)/enemy.o
	$(CC) -c $(SRC)/bomb.cpp $(INCLUDE) $(CFLAGS) -o $(OBJ)/bomb.o
	$(CC) $(SRC)/main.cpp $(OBJ)/game.o $(OBJ)/map.o $(OBJ)/entity.o $(OBJ)/bomberman.o $(OBJ)/enemy.o $(OBJ)/bomb.o $(INCLUDE) $(CFLAGS) -o $(BIN)/Bomberman

create_obj:
	mkdir -p $(OBJ)
