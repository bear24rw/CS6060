all: main.cpp shader.cpp shader.h tile.cpp tile.h terrain.cpp terrain.h camera.cpp camera.h
	g++ main.cpp shader.cpp tile.cpp terrain.cpp camera.cpp -g -o main -lGL -lGLU -lsfml-graphics

run: all
	./main
