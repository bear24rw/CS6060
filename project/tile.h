#pragma once
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <vector>

#define TILE_SIZE 50

// each row
// number of rows
// one restart value at end of each row
#define MAX_ELEMS ((TILE_SIZE+1) * 2\
                  * (TILE_SIZE)\
                  + (TILE_SIZE))

class Tile {
    public:
        Tile();
        Tile(GLuint, int, int);
        void update_vertices();
        void update_indices();
        void render();
        float get_height(int,int);
        int x, z;
    private:
        int num_elements;
        std::vector<GLuint> elements;
        std::vector<float> vertices;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
};
