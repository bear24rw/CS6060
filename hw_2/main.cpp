#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "data.h"

int main()
{

    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Homework 2", sf::Style::Default, contextSettings);
    window.setVerticalSyncEnabled(true);

    window.setActive();

    glewExperimental = GL_TRUE;
    glewInit();

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);

    GLuint shader_id = LoadShaders("vert.glsl",  "frag.glsl");
    GLint mvp_id = glGetUniformLocation(shader_id, "mvp");
    GLint posAttrib = glGetAttribLocation(shader_id, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    glm::mat4 view = glm::lookAt(
            glm::vec3(0,0,10),     // position
            glm::vec3(0,0,0),       // target
            glm::vec3(0,1,0)        // up
            );

	glm::mat4 model      = glm::mat4(1.0f);

	glm::mat4 mvp        = projection * view * model;

    while ( window.isOpen() )
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                }
            }
        }

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_id);
        glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));

        glDrawArrays(GL_LINE_STRIP, 0, sizeof(lines)/sizeof(float)/3);

        window.display();
    }

    return 0;
}

