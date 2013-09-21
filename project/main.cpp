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

#define SIZE 100
#define SPEED (100.0f)

glm::vec3 cam_pos = glm::vec3(0,10,0);
float cam_angle_horz = 0.0f;
float cam_angle_vert = 0.0f;
float cam_fov = 45.0f;
glm::vec3 cam_direction(
    cos(cam_angle_vert) * sin(cam_angle_horz),
    sin(cam_angle_vert),
    cos(cam_angle_vert) * cos(cam_angle_horz)
);
glm::vec3 cam_right(
    sin(cam_angle_horz - 3.14f/2.0f),
    0,
    cos(cam_angle_horz - 3.14f/2.0f)
);
glm::vec3 cam_up(glm::cross(cam_right,cam_direction));
glm::mat4 proj_matrix;

float get_height(int x, int y) {
    x -= SIZE/2.0f;
    y -= SIZE/2.0f;
    //return (x*x)+(y*y)/50.0f;
    return glm::simplex(glm::vec2(x,y)/80.0f)*1.9f;
}

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

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers( 1, &vbo );

    float vertices[SIZE*SIZE*(3+3+3)];
    int offset = 0;
    for (int y=0; y<SIZE; y++) {
        for (int x=0; x<SIZE; x++) {
            //position
            vertices[offset++] = x/3.f;
            vertices[offset++] = get_height(x,y);
            vertices[offset++] = y/3.f;
            // normal
            float l = get_height(x-1,y);
            float r = get_height(x+1,y);
            float d = get_height(x,y-1);
            float u = get_height(x,y+1);
            glm::vec3 n = glm::vec3(l-r, d-u, 2.0);
            float len = n.length();
            vertices[offset++] = n.x / len;
            vertices[offset++] = n.y / len;
            vertices[offset++] = n.z / len;
            // color
            vertices[offset++] = 1.0f;//x / SIZE;
            vertices[offset++] = 0.0f;//y / SIZE;
            vertices[offset++] = 0.0f;
        }
    }

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

    // Create an element array
    GLuint ebo;
    glGenBuffers( 1, &ebo );

    GLuint elements[SIZE*2*(SIZE-1)+SIZE];
    offset = 0;
    for (int y=0; y<SIZE-1; y++) {
        for (int x=0; x<SIZE; x++) {
            elements[offset++] = x+y*SIZE;
            elements[offset++] = (x+y*SIZE) + SIZE;
        }
        // restart value
        elements[offset++] = SIZE*SIZE;
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( elements ), elements, GL_STATIC_DRAW );
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(SIZE*SIZE);

    GLuint shaderID = LoadShaders("vert.glsl",  "frag.glsl");

    GLint posAttrib = glGetAttribLocation( shaderID, "position_model" );
    GLint norAttrib = glGetAttribLocation( shaderID, "normal_model" );
    GLint colAttrib = glGetAttribLocation( shaderID, "color" );
    glEnableVertexAttribArray( posAttrib );
    glEnableVertexAttribArray( norAttrib );
    glEnableVertexAttribArray( colAttrib );
    glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 9*sizeof( float ), 0 );
    glVertexAttribPointer( norAttrib, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer( colAttrib, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(6*sizeof(float)));

    glUseProgram(shaderID);
    GLint matrix_id = glGetUniformLocation(shaderID, "MVP");
    GLint view_matrix_id = glGetUniformLocation(shaderID, "V");
    GLint model_matrix_id = glGetUniformLocation(shaderID, "M");
    GLint light_id = glGetUniformLocation(shaderID, "light_pos_world");

    glm::mat4 proj_matrix = glm::perspective(cam_fov, 800.0f/600.0f, 0.1f, 100.0f);

    int mouse_pan = 0;
    int mouse_tilt = 0;
    int last_x = 0;
    int last_y = 0;

    sf::Clock clock;
    while ( window.isOpen() )
    {
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
                        cam_pos += cam_direction * delta.asSeconds() * SPEED;
                        break;
                    case sf::Keyboard::S:
                        cam_pos -= cam_direction * delta.asSeconds() * SPEED;
                        break;
                    case sf::Keyboard::A:
                        cam_pos -= cam_right * delta.asSeconds() * SPEED;
                        break;
                    case sf::Keyboard::D:
                        cam_pos += cam_right * delta.asSeconds() * SPEED;
                        break;
                    case sf::Keyboard::PageUp:
                        cam_pos += cam_up * delta.asSeconds() * SPEED;
                        break;
                    case sf::Keyboard::PageDown:
                        cam_pos -= cam_up * delta.asSeconds() * SPEED;
                        break;
                }
            } else if (event.type == sf::Event::MouseWheelMoved) {
                cam_pos += cam_direction * (float)event.mouseWheel.delta;
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
                    cam_angle_vert += (float)dy * delta.asSeconds() * 0.1f;
                    cam_angle_horz += (float)dx * delta.asSeconds() * 0.1f;
                }

                last_x = event.mouseMove.x;
                last_y = event.mouseMove.y;

            }

            // Adjust the viewport when the window is resized
            if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
                proj_matrix = glm::perspective(45.0f, (float)event.size.width/(float)event.size.height, 0.01f, 10000.0f);
            }


        }

        // update camera vectors
        cam_direction = glm::vec3(
                    cos(cam_angle_vert) * sin(cam_angle_horz),
                    sin(cam_angle_vert),
                    cos(cam_angle_vert) * cos(cam_angle_horz)
                );

        cam_right = glm::vec3(
                    sin(cam_angle_horz - 3.14f/2.0f),
                    0,
                    cos(cam_angle_horz - 3.14f/2.0f)
                );

        cam_up = glm::cross(cam_right, cam_direction);

        // clear the screen to black
        glClearColor(0.5, 0.5, 0.5, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // use our shader
        glUseProgram(shaderID);

        glm::mat4 view_matrix = glm::lookAt(cam_pos, cam_pos+cam_direction, cam_up);
        glm::mat4 model_matrix = glm::mat4(1.0f);
        glm::mat4 MVP = proj_matrix * view_matrix * model_matrix;

        glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, &model_matrix[0][0]);
        glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, &view_matrix[0][0]);


        glm::vec3 light_pos = glm::vec3(50,15,50);
        glUniform3f(light_id, light_pos.x, light_pos.y, light_pos.z);


        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Draw a rectangle from the 2 triangles using 6 indices
        glDrawElements( GL_TRIANGLE_STRIP, SIZE*2*(SIZE-1)+SIZE, GL_UNSIGNED_INT, 0 );

        // Swap buffers
        window.display();
    }

    glDeleteProgram( shaderID );

    glDeleteBuffers( 1, &ebo );
    glDeleteBuffers( 1, &vbo );

    glDeleteVertexArrays( 1, &vao );

    return 0;
}
