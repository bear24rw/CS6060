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

#include "Camera.h"

#define SIZE 500
#define SPEED (100.0f)

// Shader sources
const char* vertexSource =
    "#version 150\n"
    "in vec3 position;"
    "in vec3 color;"
    "out vec3 Color;"
    "uniform mat4 model;"
    "uniform mat4 camera;"
    "void main() {"
    "   Color = color;"
    "   gl_Position = camera * model * vec4( position, 1.0 );"
    "}";
const char* fragmentSource =
    "#version 150\n"
    "in vec3 Color;"
    "out vec4 outColor;"
    "void main() {"
    "   outColor = vec4(Color,1.0);"
    "}";

// camera
tdogl::Camera camera;

float get_height(int x, int y) {
    x -= SIZE/2.0f;
    y -= SIZE/2.0f;
    //return (x*x)+(y*y)/50.0f;
    return glm::simplex(glm::vec2(x,y)/20.0f);
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

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers( 1, &vbo );

    float vertices[SIZE*SIZE*(3+3)];
    int offset = 0;
    for (int y=0; y<SIZE; y++) {
        for (int x=0; x<SIZE; x++) {
            //position
            vertices[offset++] = x/3.f;
            vertices[offset++] = get_height(x,y);
            vertices[offset++] = y/3.f;
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


    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertexShader, 1, &vertexSource, NULL );
    glCompileShader( vertexShader );

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragmentShader, 1, &fragmentSource, NULL );
    glCompileShader( fragmentShader );

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader( shaderProgram, vertexShader );
    glAttachShader( shaderProgram, fragmentShader );
    glBindFragDataLocation( shaderProgram, 0, "outColor" );
    glLinkProgram( shaderProgram );
    glUseProgram( shaderProgram );

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
    glEnableVertexAttribArray( posAttrib );
    glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), 0 );

    GLint colAttrib = glGetAttribLocation( shaderProgram, "color" );
    glEnableVertexAttribArray( colAttrib );
    glVertexAttribPointer( colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void*)( 3 * sizeof( float ) ) );

    GLint uniModel = glGetUniformLocation( shaderProgram, "model" );
    GLint uniCamera = glGetUniformLocation( shaderProgram, "camera" );

    glm::mat4 model = glm::mat4(1.0f);
    camera.setPosition(glm::vec3(0,0,4));
    camera.setViewportAspectRatio(800.0f/600.0f);
    camera.setNearAndFarPlanes(0.01f, 10000.0f);


    int rotate = 1;
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
                    case sf::Keyboard::R:
                        rotate = !rotate;
                        break;
                    case sf::Keyboard::S:
                        camera.offsetPosition(SPEED * delta.asSeconds() * -camera.forward());
                        break;
                    case sf::Keyboard::W:
                        camera.offsetPosition(SPEED * delta.asSeconds() * camera.forward());
                        break;
                    case sf::Keyboard::A:
                        camera.offsetPosition(SPEED * delta.asSeconds() * -camera.right());
                        break;
                    case sf::Keyboard::D:
                        camera.offsetPosition(SPEED * delta.asSeconds() * camera.right());
                        break;
                    case sf::Keyboard::PageUp:
                        camera.offsetPosition(SPEED * delta.asSeconds() * glm::vec3(0,1,0));
                        break;
                    case sf::Keyboard::PageDown:
                        camera.offsetPosition(SPEED * delta.asSeconds() * -glm::vec3(0,1,0));
                        break;
                }
            } else if (event.type == sf::Event::MouseWheelMoved) {
                camera.offsetPosition((float)event.mouseWheel.delta * camera.forward());
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
                    camera.offsetPosition((float)dx * glm::vec3(1.0f, 0.0f, 0.0f));
                    camera.offsetPosition((float)dy * -glm::vec3(0.0f, 0.0f, 1.0f));
                } else if (mouse_tilt){
                    camera.offsetOrientation(-dy * 0.1, -dx * 0.1);
                }

                last_x = event.mouseMove.x;
                last_y = event.mouseMove.y;

            }

            // Adjust the viewport when the window is resized
            if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
                camera.setViewportAspectRatio((float)event.size.width/(float)event.size.height);
            }


        }

        // update camera vectors

        // Clear the screen to black
        glClearColor(0.5, 0.5, 0.5, 0.0);
        glClear( GL_COLOR_BUFFER_BIT );

        // camera
        glUniformMatrix4fv( uniCamera, 1, GL_FALSE, glm::value_ptr(camera.matrix()) );

        // Calculate transformation
        if (rotate) {
            //model = glm::mat4(1.0f);
            model = glm::rotate( model, delta.asSeconds() * 45.0f, glm::vec3( 0.0f, 1.0f, 0.0f ));
        }
        glUniformMatrix4fv( uniModel, 1, GL_FALSE, glm::value_ptr( model ) );


        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Draw a rectangle from the 2 triangles using 6 indices
        glDrawElements( GL_TRIANGLE_STRIP, SIZE*2*(SIZE-1)+SIZE, GL_UNSIGNED_INT, 0 );

        // Swap buffers
        window.display();
    }

    glDeleteProgram( shaderProgram );
    glDeleteShader( fragmentShader );
    glDeleteShader( vertexShader );

    glDeleteBuffers( 1, &ebo );
    glDeleteBuffers( 1, &vbo );

    glDeleteVertexArrays( 1, &vao );

    return 0;
}
