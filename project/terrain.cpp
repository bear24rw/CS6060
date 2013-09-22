#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "tile.h"
#include "camera.h"
#include "shader.h"
#include "terrain.h"

Terrain::Terrain() {}

Terrain::Terrain(Camera *_cam)
{
    cam = _cam;

    printf("LOADING TERRAIN SHADER\n");
    shader_id = LoadShaders("sphere_vert.glsl",  "sphere_frag.glsl");

    glUseProgram(shader_id);
    mvp_id = glGetUniformLocation(shader_id, "MVP");
    view_id = glGetUniformLocation(shader_id, "V");
    model_id = glGetUniformLocation(shader_id, "M");

    for(int x=0; x<NUM_TILES; x++) {
        for(int z=0; z<NUM_TILES; z++) {
            tiles[x][z] = Tile(shader_id, x, z);
        }
    }

    texture.loadFromFile("test.png");
    sf::Texture::bind(&texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Terrain::render()
{
    glUseProgram(shader_id);
    sf::Texture::bind(&texture);

    for(int x=0; x<NUM_TILES; x++) {
        for(int z=0; z<NUM_TILES; z++) {
            // calculate position of this tile
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x*TILE_SIZE, 0, z*TILE_SIZE));
            // update the mvp matrix
            glm::mat4 mvp = cam->proj_mat() * cam->view_mat() * model;
            glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniformMatrix4fv(model_id, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(view_id, 1, GL_FALSE, glm::value_ptr(cam->view_mat()));
            // render it
            tiles[x][z].render();
        }
    }


}

