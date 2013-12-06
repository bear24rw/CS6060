#define GLEW_STATIC
#include <vector>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "grid.h"
#include "head.h"

int main()
{
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;
    contextSettings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(800, 800), "Test 1", sf::Style::Default, contextSettings);
    window.setVerticalSyncEnabled(true);

    window.setActive();

    glewExperimental = GL_TRUE;
    glewInit();

    Camera cam;
    Grid grid(&cam);
    Head head(&cam);

    int mouse_down = 0;
    int last_mouse_x = 0;
    int last_mouse_y = 0;

    int poly_mode = 0;
    int rotate_mode = 0;

    sf::Clock clock;

    printf("Starting render loop\n");
    while (window.isOpen())
    {
        sf::Time delta = clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::P:
                        poly_mode = !poly_mode;
                        break;
                    case sf::Keyboard::Space:
                        rotate_mode = !rotate_mode;
                        break;
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left)
                    mouse_down = 1;
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left)
                    mouse_down = 0;
            }
            if (event.type == sf::Event::MouseMoved) {
                int dx = event.mouseMove.x - last_mouse_x;
                int dy = event.mouseMove.y - last_mouse_y;

                if (mouse_down){
                    cam.increase_angle_v((float)dy * delta.asSeconds() * 0.2f);
                    cam.increase_angle_h((float)dx * delta.asSeconds() * 0.2f * -1.0f);
                }

                last_mouse_x = event.mouseMove.x;
                last_mouse_y = event.mouseMove.y;

            }
            if (event.type == sf::Event::MouseWheelMoved) {
                cam.increase_distance(-event.mouseWheel.delta);
            }
            if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
                cam.set_size(event.size.width, event.size.height);
            }

        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0, 1.0, 1.0, 0.0);

        if (poly_mode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glLineWidth(2);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glLineWidth(1);
        }

        if (rotate_mode)
            head.rotation += 10.0f * delta.asSeconds();

        grid.render();
        head.render();

        window.display();
    }

    return 0;
}

