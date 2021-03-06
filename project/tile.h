#pragma once
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <vector>

#define TILE_SIZE 128

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
        void set_xz(int,int);
        int world_x(void);
        int world_z(void);
        bool needs_rebuild;
        bool needs_gen;
        int tile_x;
        int tile_z;
        float height[TILE_SIZE+1][TILE_SIZE+1];
        void lod(int);
        int lod(void);
        bool show;
    private:
        int _lod;
        int lod_step;
        int num_elements;
        std::vector<GLuint> elements;
        std::vector<float> vertices;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        bool reload_vbo;
        bool reload_ebo;
        sf::Mutex vbo_mutex;
        sf::Mutex ebo_mutex;
        void update();
};
