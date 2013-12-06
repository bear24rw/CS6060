#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "shader.h"
#include "objloader.h"
#include "head.h"

Head::Head() {}

Head::Head(Camera *_cam)
{
    cam = _cam;

    rotation = 0.0f;

    shader_id = LoadShaders("head_vert.glsl", "head_frag.glsl");

    model_id = glGetUniformLocation(shader_id, "M");
    view_id  = glGetUniformLocation(shader_id, "V");
    proj_id  = glGetUniformLocation(shader_id, "P");
    light_id = glGetUniformLocation(shader_id, "light_pos_world");
    texture_id = glGetUniformLocation(shader_id, "tex");

    texture.loadFromFile("../assets/uvmap.png");

    // read the obj file
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    load_obj("../assets/head.obj", indices, indexed_vertices, indexed_uvs, indexed_normals);

    offset = 0.0f;
    for (glm::vec3 i : indexed_vertices) {
        if (i.y < offset) offset = i.y;
    }

    cam->target.y = -offset * 0.75f;

    printf("[head] offset: %f\n", offset);

    GLuint posAttrib = glGetAttribLocation(shader_id, "vert_pos_model");
    GLuint norAttrib = glGetAttribLocation(shader_id, "vert_nor_model");
    GLuint texAttrib = glGetAttribLocation(shader_id, "vert_tex");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(posAttrib);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(norAttrib);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(texAttrib);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

    sf::Texture::bind(&texture);

    glBindVertexArray(0);
}

void Head::render()
{
    glUseProgram(shader_id);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, rotation, glm::vec3(0,1,0));
    model = glm::translate(model, glm::vec3(0.0f, -offset, 0.0f));

    glUniformMatrix4fv(proj_id, 1, GL_FALSE, glm::value_ptr(cam->proj_mat()));
    glUniformMatrix4fv(view_id, 1, GL_FALSE, glm::value_ptr(cam->view_mat()));
    glUniformMatrix4fv(model_id, 1, GL_FALSE, glm::value_ptr(model));

    glUniform3f(light_id, cam->position.x, cam->position.y, cam->position.z);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
}
