#pragma once

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "tile.h"
#include "camera.h"

#define NUM_TILES 5

class Terrain {
    public:
        Terrain();
        Terrain(Camera*);
        void render(void);
        void update(void);
    private:
        //std::vector<Tile*> tiles;
        Tile *tiles[NUM_TILES][NUM_TILES];
        GLuint shader_id;
        GLuint mvp_id;
        GLuint view_id;
        GLuint model_id;
        sf::Texture texture;
        Camera *cam;
};
