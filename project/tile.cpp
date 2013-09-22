#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/gtc/noise.hpp>
#include <vector>

#include "tile.h"

Tile::Tile() {}

Tile::Tile(GLuint shader_id, int _x, int _z)
{
    x = _x;
    z = _z;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    GLuint posAttrib = glGetAttribLocation(shader_id, "position_model");
    GLuint norAttrib = glGetAttribLocation(shader_id, "normal_model");
    GLuint texAttrib = glGetAttribLocation(shader_id, "texcoord");
    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(norAttrib);
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0 );
    glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));

    update_vertices();
    update_indices();
}

float Tile::get_height(int _x, int _z) {
    //x -= SIZE/2.0f;
    //y -= SIZE/2.0f;
    //return ((x*x)+(y*y))/100.0f;
    //return glm::simplex(glm::vec2(x,y)/50.0f)*3.9f;
    _x += (x * TILE_SIZE);
    _z += (z * TILE_SIZE);
    return glm::simplex(glm::vec2(_x,_z)/50.0f)*2.0f +
           fabs(glm::simplex(glm::vec2(_x,_z)/400.0f)*20.0f) +
           glm::simplex(glm::vec2(_x,_z)/800.0f)*100.0f;

    //return fabs(glm::simplex(glm::vec2(x,y)/50.0f)*3.9f);
    //return 0;
}

void Tile::update_indices()
{
    elements.clear();

    for (int z=0; z<=TILE_SIZE-1; z++) {
        for (int x=0; x<=TILE_SIZE; x++) {
            elements.push_back(x+z*(TILE_SIZE+1));
            elements.push_back((x+z*(TILE_SIZE+1)) + (TILE_SIZE+1));
        }
        // restart value
        elements.push_back((TILE_SIZE+1)*(TILE_SIZE+1)+TILE_SIZE);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(GLuint), elements.data(), GL_STATIC_DRAW);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex((TILE_SIZE+1)*(TILE_SIZE+1)+TILE_SIZE);
}

void Tile::update_vertices()
{
    vertices.clear();

    int offset = 0;
    for (int z=0; z<=TILE_SIZE; z++) {
        for (int x=0; x<=TILE_SIZE; x++) {
            //position
            vertices.push_back(x);
            vertices.push_back(get_height(x,z));
            vertices.push_back(z);
            // normal
            float l = get_height(x-1,z);
            float r = get_height(x+1,z);
            float d = get_height(x,z-1);
            float u = get_height(x,z+1);
            glm::vec3 n = glm::normalize(glm::vec3(l-r, 2.0, d-u));
            vertices.push_back(n.x);
            vertices.push_back(n.y);
            vertices.push_back(n.z);
            // texture
            vertices.push_back((float)(x%2));
            vertices.push_back((float)((z+1)%2));
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

void Tile::render()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glDrawElements(GL_TRIANGLE_STRIP, elements.size(), GL_UNSIGNED_INT, 0);
}

