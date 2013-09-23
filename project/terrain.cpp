#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
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
            Tile *t = new Tile(shader_id, x, z);
            tiles[x][z] = t;
        }
    }

    texture.loadFromFile("sand.png");
    sf::Texture::bind(&texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Terrain::update()
{
    sf::Clock clock;
    clock.restart();

    int padding = 1;

    int cam_x = cam->position.x / TILE_SIZE;
    int cam_z = cam->position.z / TILE_SIZE;

    int dist;

    for(int z=0; z<NUM_TILES; z++) {
        for(int x=0; x<NUM_TILES; x++) {

            dist = cam_z - tiles[x][z]->tile_z;

            if (abs(dist) > NUM_TILES-padding) {

                int furthest = tiles[x][z]->tile_z;

                if (dist > 0) {
                    // we are moving z+
                    // find most furthest north tile in this row
                    for(int zz=0; zz<NUM_TILES; zz++) {
                        if (tiles[x][zz]->tile_z > furthest)
                            furthest = tiles[x][zz]->tile_z;
                    }

                    // reassing current tile to 1 past furthest
                    printf("Reassinging: (%d,%d) -> (%d,%d)\n", x,z,x,furthest+1);
                    tiles[x][z]->set_xz(x, furthest+1);
                } else {
                    // we are moving z-
                    // find most furthest south tile in this row
                    for(int zz=0; zz<NUM_TILES; zz++) {
                        if (tiles[x][zz]->tile_z < furthest)
                            furthest = tiles[x][zz]->tile_z;
                    }

                    // reassing current tile to 1 past furthest
                    printf("Reassinging: (%d,%d) -> (%d,%d)\n", x,z,x,furthest-1);
                    tiles[x][z]->set_xz(x, furthest-1);
                }
            }


            dist = cam_x - tiles[x][z]->tile_x;

            if (abs(dist) > NUM_TILES-padding) {

                int furthest = tiles[x][z]->tile_x;

                if (dist > 0) {
                    // we are moving z+
                    // find most furthest north tile in this row
                    for(int xx=0; xx<NUM_TILES; xx++) {
                        if (tiles[xx][z]->tile_x > furthest)
                            furthest = tiles[xx][z]->tile_x;
                    }

                    // reassing current tile to 1 past furthest
                    printf("Reassinging: (%d,%d) -> (%d,%d)\n", x,z,furthest+1,z);
                    tiles[x][z]->set_xz(furthest+1, z);
                } else {
                    // we are moving z-
                    // find most furthest south tile in this row
                    for(int xx=0; xx<NUM_TILES; xx++) {
                        if (tiles[xx][z]->tile_x < furthest)
                            furthest = tiles[xx][z]->tile_x;
                    }

                    // reassing current tile to 1 past furthest
                    printf("Reassinging: (%d,%d) -> (%d,%d)\n", x,z,furthest-1,z);
                    tiles[x][z]->set_xz(furthest-1, z);
                }
            }

        }
    }

    printf("%fs\n", clock.restart().asSeconds());

}

void Terrain::render()
{

    glUseProgram(shader_id);
    sf::Texture::bind(&texture);

    for(int x=0; x<NUM_TILES; x++) {
        for(int z=0; z<NUM_TILES; z++) {
            if (tiles[x][z]->ready_to_render == false) {
                //printf("Tile %d %d not ready to render, skipping..\n");
                continue;
            }
            // absolute world position of this tile
            glm::vec3 wp(tiles[x][z]->world_x(), 0, tiles[x][z]->world_z());
            // move the model matrix to that position
            glm::mat4 model = glm::translate(glm::mat4(1.0f), wp);
            //glm::mat4 model(1.0f);
            // update the mvp matrix
            glm::mat4 mvp = cam->proj_mat() * cam->view_mat() * model;
            glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniformMatrix4fv(model_id, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(view_id, 1, GL_FALSE, glm::value_ptr(cam->view_mat()));
            // render it
            tiles[x][z]->render();
        }
    }


}

