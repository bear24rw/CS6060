#pragma once
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <vector>

#define TILE_SIZE 200

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
        void update_vertices(void);
        void update_indices(void);
        void render(void);
        float get_height(int,int);
        void set_xz(int,int);
        int world_x(void);
        int world_z(void);
        bool ready_to_render;
        int tile_x;
        int tile_z;
    private:
        int num_elements;
        std::vector<GLuint> elements;
        std::vector<float> vertices;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        sf::Thread update_thread;
        void update();
};
