TARGET = main
LIBS = -lGL -lGLU -lsfml-graphics -lsfml-system -lsfml-window -lGLEW
CC = g++
CFLAGS = -O3 -Wall

.PHONY: default all clean run

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

#.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS) $(HEADERS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)

run: all
	./main
