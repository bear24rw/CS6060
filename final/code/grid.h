#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <vector>
#include "camera.h"

class Grid {
    public:
        Grid();
        Grid(Camera*);
        void render();
    private:
        Camera *cam;
        std::vector<float> verts;
        GLuint vao;
        GLuint vbo;
        GLuint shader_id;
        GLuint mvp_id;
};
