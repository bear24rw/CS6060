#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <glm/gtc/noise.hpp>

#include "tile.h"

Tile::Tile()
{ }

Tile::Tile(GLuint shader_id, int x, int z)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    GLuint posAttrib = glGetAttribLocation(shader_id, "position_model");
    GLuint norAttrib = glGetAttribLocation(shader_id, "normal_model");
    GLuint texAttrib = glGetAttribLocation(shader_id, "texcoord");
    GLuint colAttrib = glGetAttribLocation(shader_id, "color");
    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(norAttrib);
    glEnableVertexAttribArray(texAttrib);
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), 0 );
    glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(6*sizeof(float)));
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(8*sizeof(float)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex((TILE_SIZE+1)*(TILE_SIZE+1)+TILE_SIZE);

    reload_vbo = false;
    reload_ebo = false;
    needs_rebuild = false;
    needs_gen = true;
    show = false;

    lod(0);
    set_xz(x,z);
    update_vertices();
    update_indices();
}

void Tile::lod(int l)
{
    _lod = l;
    lod_step = pow(2, _lod);
}

int Tile::lod(void) {return _lod;}

void Tile::set_xz(int x, int z)
{
    tile_x = x;
    tile_z = z;
    needs_gen = true;
}

int Tile::world_x() { return (tile_x * TILE_SIZE); }
int Tile::world_z() { return (tile_z * TILE_SIZE); }


void Tile::update_indices()
{
    ebo_mutex.lock();
    elements.clear();

    int verts_per_side = TILE_SIZE/lod_step + 1;

    for (int z=0; z<verts_per_side-1; z++) {
        for (int x=0; x<verts_per_side; x++) {
            elements.push_back(x+z*verts_per_side);
            elements.push_back((x+z*verts_per_side) + verts_per_side);
        }
        // restart value
        elements.push_back((TILE_SIZE+1)*(TILE_SIZE+1)+TILE_SIZE);
    }
    reload_ebo = true;
    ebo_mutex.unlock();

}

void Tile::update_vertices()
{
    sf::Clock clock;
    clock.restart();

    float lod_color[6][3] = {
        {1.0f, 0.8f, 0.8f},     // 1 - red
        {0.8f, 1.0f, 0.8f},     // 2 - green
        {0.8f, 0.8f, 1.0f},     // 4 - blue
        {1.0f, 0.8f, 1.0f},     // 8 - purple
        {1.0f, 1.0f, 0.8f},     // 16 - yellow
        {0.8f, 1.0f, 1.0f},     // 32 - teal
    };

    vbo_mutex.lock();
    vertices.clear();

    int offset = 0;
    for (int z=0; z<=TILE_SIZE; z += lod_step) {
        for (int x=0; x<=TILE_SIZE; x += lod_step) {
            //position
            vertices.push_back(x);
            vertices.push_back(height[x][z]);
            vertices.push_back(z);
            // normal
            float l = height[std::max(0, x-1)][z];
            float r = height[std::min(TILE_SIZE, x+1)][z];
            float d = height[x][std::max(0, z-1)];
            float u = height[x][std::min(TILE_SIZE, z+1)];
            glm::vec3 n = glm::normalize(glm::vec3(l-r, 2.0, d-u));
            vertices.push_back(n.x);
            vertices.push_back(n.y);
            vertices.push_back(n.z);
            // texture
            vertices.push_back((float)(x%2));
            vertices.push_back((float)((z+1)%2));
            // color
            vertices.push_back(lod_color[_lod][0]);
            vertices.push_back(lod_color[_lod][1]);
            vertices.push_back(lod_color[_lod][2]);
            //vertices.push_back(1.0f);
            //vertices.push_back(1.0f);
            //vertices.push_back(1.0f);
        }
    }
    reload_vbo = true;
    vbo_mutex.unlock();
    //printf("Update took: %fs\n", clock.restart().asSeconds());
}

void Tile::render()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    vbo_mutex.lock();
    if (reload_vbo) {
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
        reload_vbo = false;
    }
    vbo_mutex.unlock();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    ebo_mutex.lock();
    if (reload_ebo) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(GLuint), elements.data(), GL_STATIC_DRAW);
        reload_ebo = false;
    }
    ebo_mutex.unlock();
    glDrawElements(GL_TRIANGLE_STRIP, elements.size(), GL_UNSIGNED_INT, 0);
}

