#include <queue>
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

Terrain::Terrain()
: update_thread(&Terrain::update, this)
{}

Terrain::Terrain(Camera *_cam)
: update_thread(&Terrain::update, this)
{
    cam = _cam;

    printf("LOADING TERRAIN SHADER\n");
    shader_id = LoadShaders("sphere_vert.glsl",  "sphere_frag.glsl");

    glUseProgram(shader_id);
    mvp_id = glGetUniformLocation(shader_id, "MVP");
    view_id = glGetUniformLocation(shader_id, "V");
    model_id = glGetUniformLocation(shader_id, "M");

    for(int x=0; x<NUM_TILES_X; x++) {
        for(int z=0; z<NUM_TILES_Z; z++) {
            printf("Generating [%d/%d]\n", x*NUM_TILES_X+z, NUM_TILES);
            Tile *t = new Tile(shader_id, x, z);
            tiles[x*NUM_TILES_X+z] = t;
        }
    }

    texture.loadFromFile("sand.png");
    sf::Texture::bind(&texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    update_thread.launch();
}

float Terrain::get_height(int x, int z) {
    float a = fabs(glm::simplex(glm::vec2(x,z)/280.0f)*10.0f);
    float b = glm::simplex(glm::vec2(x,z)/410.0f)*0.09f;
    float c = glm::simplex(glm::vec2(x,z)/809.0f)*3.0f;
    float d = fabs(glm::simplex(glm::vec2(x,z)/600.0f)*30.0f);
    //if (b < 0) b = 0.0f;
    //if (c < 0) c = 0.0f;
    //if (d < 0) d = 0.0f;
    //if (d > 180.0f) d = 180.0f;

    //return a+b+c+d;
    return a;
    //return 5;

    //return fabs(glm::simplex(glm::vec2(x,y)/50.0f)*3.9f);
    //return 0;
}

void Terrain::update()
{
    printf("Terrain update thread started\n");

    sf::Clock clock;
    clock.restart();

    int last_cam_x = 100000;
    int last_cam_z = 100000;

    while (1) {

        int cam_x = cam->position.x / TILE_SIZE;
        int cam_z = cam->position.z / TILE_SIZE;

        // if we haven't moved sleep and try again later
        if (cam_x == last_cam_x && cam_z == last_cam_z) {
            sf::sleep(sf::milliseconds(100));
            continue;
        }

        printf("Updating tiles\n");

        // we have moved so save the new values
        last_cam_x = cam_x;
        last_cam_z = cam_z;

        // grid centered around player, assumed to be empty
        int slots[NUM_TILES_X][NUM_TILES_Z];
        memset(slots, -1, sizeof(slots));

        std::queue<int> free;

        // fill out grid and keep track of free'd tiles
        for(int i=0; i<NUM_TILES; i++) {
            int tx = tiles[i]->tile_x;
            int tz = tiles[i]->tile_z;
            int dist_x = tx - cam_x;
            int dist_z = tz - cam_z;
            if (abs(dist_x) > NUM_TILES_X/2 || abs(dist_z) > NUM_TILES_Z/2) {
                //printf("Tile %d at (%d,%d) is (%d,%d) from cam (%d,%d)\n", i, tx, tz, dist_x, dist_z, cam_x, cam_z);
                free.push(i);
            } else {
                // the camera is at the center of 'slots' so add the distance to the center
                slots[dist_x+(NUM_TILES_X/2)][dist_z+(NUM_TILES_Z/2)] = i;
            }
        }

        // loop through grid and try to fill empty slots with free tiles
        for(int x=0; x<NUM_TILES_X; x++) {
            for(int z=0; z<NUM_TILES_Z; z++) {
                // if slot is filled continue
                if (slots[x][z] > -1) continue;

                if (free.empty()) {
                    //printf("Not enough free tiles to fill all slots!\n");
                    break;
                }

                int new_x = cam_x + (x - NUM_TILES_X/2);
                int new_z = cam_z + (z - NUM_TILES_Z/2);
                //printf("Setting %d to (%d,%d)\n", free.front(), new_x, new_z);
                tiles[free.front()]->set_xz(new_x,new_z);
                free.pop();
            }
        }

        if (free.empty() == false) printf("Too many free tiles!\n");

        // loop through all tiles and see if they need to be rebuilt
        for(int i=0; i<NUM_TILES; i++) {
            if (tiles[i]->needs_rebuild == false) continue;
            printf("Rebuilding tile %d\n", i);

            // update the heightmap
            for(int x=0; x<=TILE_SIZE; x++) {
                for(int z=0; z<=TILE_SIZE; z++) {
                    int wx = tiles[i]->world_x() + x;
                    int wz = tiles[i]->world_z() + z;
                    tiles[i]->height[x][z] = get_height(wx, wz);
                }
            }

            // update the vertices
            tiles[i]->update_vertices();
            tiles[i]->needs_rebuild = false;
        }

        //printf("%fs\n", clock.restart().asSeconds());
    }

}

void Terrain::render()
{

    glUseProgram(shader_id);
    sf::Texture::bind(&texture);

    for(int i=0; i<NUM_TILES; i++) {
        // absolute world position of this tile
        glm::vec3 wp(tiles[i]->world_x(), 0, tiles[i]->world_z());
        // move the model matrix to that position
        glm::mat4 model = glm::translate(glm::mat4(1.0f), wp);
        //glm::mat4 model(1.0f);
        // update the mvp matrix
        glm::mat4 mvp = cam->proj_mat() * cam->view_mat() * model;
        glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(model_id, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_id, 1, GL_FALSE, glm::value_ptr(cam->view_mat()));
        // render it
        tiles[i]->render();
    }


}

