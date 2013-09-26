// Link statically with GLEW
#define GLEW_STATIC

// Headers
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include "shader.h"
#include "tile.h"
#include "terrain.h"
#include "skybox.h"

#define SPEED (50.0f)

int main()
{
    // Request a 32-bits depth buffer when creating the window
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML graphics with OpenGL", sf::Style::Default, contextSettings);
    window.setVerticalSyncEnabled(true);

    // Make it the active window for OpenGL calls
    window.setActive();

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);

    // debug lines
    GLuint default_vao;
    glGenVertexArrays(1, &default_vao);
    glBindVertexArray(default_vao);
    GLuint default_vbo;
    glGenBuffers(1, &default_vbo);

    float lines[] = {
        // xyz color
        0.0f,   0.0f,   0.0f,   1.0f, 0.0f, 0.0f,
        (TILE_SIZE-1),   0.0f,   0.0f,   1.0f, 0.0f, 0.0f,

        (TILE_SIZE-1),   0.0f,   0.0f,   1.0f, 0.0f, 0.0f,
        (TILE_SIZE-1),   0.0f,   (TILE_SIZE-1),  1.0f, 0.0f, 0.0f,

        (TILE_SIZE-1),   0.0f,   (TILE_SIZE-1),   1.0f, 0.0f, 0.0f,
        0.0f,    0.0f,   (TILE_SIZE-1),   1.0f, 0.0f, 0.0f,

        0.0f,    0.0f,   (TILE_SIZE-1),   1.0f, 0.0f, 0.0f,
        0.0f,    0.0f,   0.0f,    1.0f, 0.0f, 0.0f,
        // axis
        0.0f,   0.0f,   0.0f,   0.0f, 0.0f, 1.0f,
        2.0f*(TILE_SIZE-1),   0.0f,   0.0f,  0.0f, 0.0f, 1.0f,

        0.0f,    0.0f,   0.0f,   1.0f, 0.0f, 0.0f,
        0.0f,    2.0f*(TILE_SIZE-1),   0.0f,    1.0f, 0.0f, 0.0f,

        0.0f,   0.0f,   0.0f,   0.0f, 1.0f, 0.0f,
        0.0f,    0.0f,   2.0f*(TILE_SIZE-1),   0.0f, 1.0f, 0.0f
    };
/*
    float lines[] = {
        // xyz color
        0.0f  , 0.0f , 0.0f  , 1.0f , 0.0f , 0.0f ,
        50.0f , 0.0f , 50.0f , 1.0f , 0.0f , 0.0f ,
        50.0f , 0.0f , 0.0f  , 1.0f , 0.0f , 0.0f ,
    };
    */
    glBindBuffer(GL_ARRAY_BUFFER, default_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);


    glBindVertexArray(default_vao);
    glBindBuffer(GL_ARRAY_BUFFER, default_vbo);
    printf("LOADING DEFAULT SHADER\n");
    GLuint shader_id = LoadShaders("default_vert.glsl",  "default_frag.glsl");
    GLint posAttrib = glGetAttribLocation(shader_id, "position_model");
    GLint colAttrib = glGetAttribLocation(shader_id, "color");
    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));

    glUseProgram(shader_id);
    GLint mvp_id = glGetUniformLocation(shader_id, "MVP");
    GLint view_id = glGetUniformLocation(shader_id, "V");
    GLint model_id = glGetUniformLocation(shader_id, "M");

    Camera cam;
    Terrain terrain(&cam);
    Skybox skybox(&cam);

    int mouse_pan = 0;
    int mouse_tilt = 0;
    int last_x = 0;
    int last_y = 0;

    int draw_polys = 0;
    int draw_norms = 0;

    bool key_a = false;
    bool key_s = false;
    bool key_w = false;
    bool key_d = false;
    bool key_up = false;
    bool key_down = false;

    sf::Clock clock;
    sf::Clock fps_counter;
    while ( window.isOpen() )
    {
        //float fps = 1.0f / fps_counter.restart().asSeconds();
        //printf("fps: %f\n", fps);

        sf::Time delta = clock.restart();

        // Process events
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
                    case sf::Keyboard::W:
                        key_w = true;
                        break;
                    case sf::Keyboard::S:
                        key_s = true;
                        break;
                    case sf::Keyboard::A:
                        key_a = true;
                        break;
                    case sf::Keyboard::D:
                        key_d = true;
                        break;
                    case sf::Keyboard::PageUp:
                        key_up = true;
                        break;
                    case sf::Keyboard::PageDown:
                        key_down = true;
                        break;
                    case sf::Keyboard::P:
                        draw_polys = !draw_polys;
                        break;
                    case sf::Keyboard::N:
                        draw_norms = !draw_norms;
                        break;
                }
            } else if (event.type == sf::Event::KeyReleased) {
                switch(event.key.code) {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::W:
                        key_w = false;
                        break;
                    case sf::Keyboard::S:
                        key_s = false;
                        break;
                    case sf::Keyboard::A:
                        key_a = false;
                        break;
                    case sf::Keyboard::D:
                        key_d = false;
                        break;
                    case sf::Keyboard::PageUp:
                        key_up = false;
                        break;
                    case sf::Keyboard::PageDown:
                        key_down = false;
                        break;
                }
            } else if (event.type == sf::Event::MouseWheelMoved) {
                cam.translate(cam.direction * (float)event.mouseWheel.delta);
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Middle) mouse_pan = 1;
                if (event.mouseButton.button == sf::Mouse::Left) mouse_tilt = 1;
            } else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Middle) mouse_pan = 0;
                if (event.mouseButton.button == sf::Mouse::Left) mouse_tilt = 0;
            } else if (event.type == sf::Event::MouseMoved) {
                int dx = event.mouseMove.x - last_x;
                int dy = event.mouseMove.y - last_y;

                if (mouse_pan) {
                } else if (mouse_tilt){
                    cam.increase_angle_v((float)dy * delta.asSeconds() * 0.1f);
                    cam.increase_angle_h((float)dx * delta.asSeconds() * 0.1f);
                }

                last_x = event.mouseMove.x;
                last_y = event.mouseMove.y;
            }

            // Adjust the viewport when the window is resized
            if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
                cam.set_size(event.size.width, event.size.height);
            }

        }

        if (key_w) cam.translate(cam.direction * delta.asSeconds() * SPEED);
        if (key_s) cam.translate(-cam.direction * delta.asSeconds() * SPEED);
        if (key_a) cam.translate(-cam.right * delta.asSeconds() * SPEED);
        if (key_d) cam.translate(cam.right * delta.asSeconds() * SPEED);
        if (key_up) cam.translate(cam.up * delta.asSeconds() * SPEED);
        if (key_down) cam.translate(-cam.up * delta.asSeconds() * SPEED);

        // clear the screen to black
        glClearColor(0.5, 0.5, 0.5, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glm::vec3 light_pos = glm::vec3(TILE_SIZE/2,20,TILE_SIZE/2);
        //glUniform3f(light_id, light_pos.x, light_pos.y, light_pos.z);

        if (draw_polys) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        skybox.render();
        terrain.render();

        // normal shader
        /*
        if (draw_norms) {
            glUseProgram(norm_shader_id);
            glUniformMatrix4fv(debug_matrix_id, 1, GL_FALSE, &MVP[0][0]);
            glDrawElements( GL_TRIANGLE_STRIP, TILE_SIZE*2*(TILE_SIZE-1)+TILE_SIZE, GL_UNSIGNED_INT, 0 );
        }
        */

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 mvp = cam.proj_mat() * cam.view_mat() * model;

        // use our shader
        glUseProgram(shader_id);
        glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(model_id, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_id, 1, GL_FALSE, glm::value_ptr(cam.view_mat()));

        // debug box
        glBindVertexArray(default_vao);
        glBindBuffer(GL_ARRAY_BUFFER, default_vbo);
        glUseProgram(shader_id);
        glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glLineWidth(5);
        glDrawArrays(GL_LINES, 0, sizeof(lines)/sizeof(float)/6);

        // Swap buffers
        window.display();
    }

    //glDeleteProgram( shader_id );

    //glDeleteBuffers( 1, &ebo );
    //glDeleteBuffers( 1, &vbo );

    //glDeleteVertexArrays( 1, &vao );

    return 0;
}
