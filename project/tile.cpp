#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <glm/gtc/noise.hpp>
#include <vector>

#include "tile.h"

Tile::Tile()
: update_thread(&Tile::update, this)
{ }

Tile::Tile(GLuint shader_id, int x, int z)
: update_thread(&Tile::update, this)
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
    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(norAttrib);
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0 );
    glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));

    update_indices();

    ready_to_render = false;

    set_xz(x,z);
}

void Tile::set_xz(int x, int z)
{
    tile_x = x;
    tile_z = z;
    update_thread.launch();
}

int Tile::world_x() { return (tile_x * TILE_SIZE); }
int Tile::world_z() { return (tile_z * TILE_SIZE); }

void Tile::update()
{
    ready_to_render = false;
    update_vertices();
    ready_to_render = true;
}

float Tile::get_height(int x, int z) {
    //x -= SIZE/2.0f;
    //y -= SIZE/2.0f;
    //return ((x*x)+(y*y))/100.0f;
    //return glm::simplex(glm::vec2(x,y)/50.0f)*3.9f;

    // translate coordinates into world coordinates
    x += world_x();
    z += world_z();

    float height = 0.0f;

    float a = glm::simplex(glm::vec2(x,z)/50.0f)*2.0f;
    float b = glm::simplex(glm::vec2(x,z)/400.0f)*20.0f;
    float c = glm::simplex(glm::vec2(x,z)/800.0f)*100.0f;
    if (b < 0) b = 0.0f;
    if (c < 0) c = 0.0f;

    return a+b+c;

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
    sf::Clock clock;
    clock.restart();

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
    printf("Update took: %fs\n", clock.restart().asSeconds());
}

void Tile::render()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLE_STRIP, elements.size(), GL_UNSIGNED_INT, 0);
}

