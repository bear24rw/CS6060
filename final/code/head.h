#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <vector>
#include "camera.h"

class Head {
    public:
        Head();
        Head(Camera*);
        void render();
        float rotation;
    private:
        std::vector<unsigned short> indices;
        GLuint vao;
        GLuint vertexbuffer;
        GLuint uvbuffer;
        GLuint normalbuffer;
        GLuint elementbuffer;
        GLuint shader_id;
        GLuint model_id;
        GLuint view_id;
        GLuint proj_id;
        GLuint texture_id;
        sf::Texture texture;
        GLuint light_id;
        Camera *cam;
        float offset;
};
