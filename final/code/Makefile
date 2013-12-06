TARGET = main
LIBS = -lGL -lGLU -lsfml-graphics -lsfml-system -lsfml-window -lGLEW -g -lassimp
CC = g++
CXXFLAGS = -O3 -Wall -std=c++11

.PHONY: default all clean run

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) -c $< -o $@

#.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS) $(HEADERS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)

run: all
	./main
