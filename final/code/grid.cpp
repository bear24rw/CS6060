#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "shader.h"
#include "grid.h"

Grid::Grid() {}

Grid::Grid(Camera *_cam)
{
    cam = _cam;

    #define GRID_SIZE   20

    for(int k = -GRID_SIZE; k<GRID_SIZE; k++) {
        // start of x line / color / end of x line / color
        verts.push_back(k); verts.push_back(0); verts.push_back(-GRID_SIZE);
        verts.push_back(1); verts.push_back(0); verts.push_back(0);
        verts.push_back(k); verts.push_back(0); verts.push_back(GRID_SIZE);
        verts.push_back(1); verts.push_back(0); verts.push_back(0);
        // start of z line / color / end of z line / color
        verts.push_back(-GRID_SIZE); verts.push_back(0); verts.push_back(k);
        verts.push_back(0); verts.push_back(0); verts.push_back(1);
        verts.push_back(GRID_SIZE); verts.push_back(0); verts.push_back(k);
        verts.push_back(0); verts.push_back(0); verts.push_back(1);
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), verts.data(), GL_STATIC_DRAW);

    printf("[grid] Loading shaders\n");
    shader_id = LoadShaders("grid_vert.glsl", "grid_frag.glsl");

    mvp_id = glGetUniformLocation(shader_id, "mvp");
    GLint posAttrib = glGetAttribLocation(shader_id, "position");
    GLint colAttrib = glGetAttribLocation(shader_id, "color");
    printf("[grid] %d %d\n", posAttrib, colAttrib);
    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), 0);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

}

void Grid::render()
{
    glUseProgram(shader_id);

    glm::mat4 mvp = cam->proj_mat() * cam->view_mat() * glm::mat4(1.0f);
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, verts.size()/6-2);
}
