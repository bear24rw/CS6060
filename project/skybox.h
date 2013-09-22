#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <vector>
#include "camera.h"

class Skybox {
    public:
        Skybox();
        Skybox(Camera*);
        void render();
    private:
        std::vector<float> vertices;
        std::vector<int> indices;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        GLuint shader_id;
        GLuint mvp_id;
        GLuint view_id;
        GLuint model_id;
        GLuint texture;
        sf::Image images[6];
        Camera *cam;
};
