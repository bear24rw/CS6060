#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "shader.h"
#include "skybox.h"

Skybox::Skybox() {}

Skybox::Skybox(Camera *_cam)
{
    cam = _cam;

    printf("LOADING SKYBOX SHADER\n");
    shader_id = LoadShaders("skybox_vert.glsl", "skybox_frag.glsl");

    glUseProgram(shader_id);
    mvp_id = glGetUniformLocation(shader_id, "mvp");

    images[0].loadFromFile("skyboxes/skyboxsun25degtest/skyrender0001.bmp");  // +x
    images[1].loadFromFile("skyboxes/skyboxsun25degtest/skyrender0004.bmp");  // -x
    images[2].loadFromFile("skyboxes/skyboxsun25degtest/skyrender0003.bmp");  // +y
    images[3].loadFromFile("skyboxes/skyboxsun25degtest/skyrender0003.bmp");  // -y
    images[4].loadFromFile("skyboxes/skyboxsun25degtest/skyrender0005.bmp");  // +z
    images[5].loadFromFile("skyboxes/skyboxsun25degtest/skyrender0002.bmp");  // -z

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for(int i=0; i<6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGBA, images[i].getSize().x, images[i].getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, images[i].getPixelsPtr());
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    GLuint posAttrib = glGetAttribLocation(shader_id, "position_model");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    float s = 70.0f;

    float v[] = {
        -1.0 , 1.0  , 1.0  ,
        -1.0 , -1.0 , 1.0  ,
        1.0  , -1.0 , 1.0  ,
        1.0  , 1.0  , 1.0  ,
        -1.0 , 1.0  , -1.0 ,
        -1.0 , -1.0 , -1.0 ,
        1.0  , -1.0 , -1.0 ,
        1.0  , 1.0  , -1.0 ,
    };

    vertices.clear();
    for(int i=0; i<sizeof(v); i++) {
        vertices.push_back(v[i]);
    }

    glBufferData(GL_ARRAY_BUFFER, vertices.size(), &vertices.front(), GL_STATIC_DRAW);

    int elems[] = {
        0, 1, 2, 3,
        3, 2, 6, 7,
        7, 6, 5, 4,
        4, 5, 1, 0,
        0, 3, 7, 4,
        1, 2, 6, 5,
    };

    indices.clear();
    for(int i=0; i<sizeof(elems); i++) {
        indices.push_back(elems[i]);
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size(), &indices.front(), GL_STATIC_DRAW);
}

void Skybox::render()
{
    glUseProgram(shader_id);

    // center the model on the camera
    glm::mat4 model = glm::translate(glm::mat4(1.0f), cam->position);
    //glm::mat4 model(1.0f);
    glm::mat4 mvp = cam->proj_mat() * cam->view_mat() * model;
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, 0);
    glEnable(GL_DEPTH_TEST);
}
