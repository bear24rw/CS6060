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
#include "shader.h"

int main()
{
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Test 1", sf::Style::Default, contextSettings);
    window.setVerticalSyncEnabled(true);

    window.setActive();

    glewExperimental = GL_TRUE;
    glewInit();

    //
    // Setup the grid
    //

    #define GRID_SIZE   10

    std::vector<float> default_verts;

    for(int k = 0; k<GRID_SIZE; k++) {
        // start of k line
        default_verts.push_back(k);
        default_verts.push_back(0);
        default_verts.push_back(0);
        //color
        default_verts.push_back(1);
        default_verts.push_back(0);
        default_verts.push_back(0);
        // end of k line
        default_verts.push_back(k);
        default_verts.push_back(0);
        default_verts.push_back(GRID_SIZE);
        //color
        default_verts.push_back(1);
        default_verts.push_back(0);
        default_verts.push_back(0);
        // start of z line
        default_verts.push_back(0);
        default_verts.push_back(0);
        default_verts.push_back(k);
        //color
        default_verts.push_back(0);
        default_verts.push_back(0);
        default_verts.push_back(1);
        // end of z line
        default_verts.push_back(GRID_SIZE);
        default_verts.push_back(0);
        default_verts.push_back(k);
        //color
        default_verts.push_back(0);
        default_verts.push_back(0);
        default_verts.push_back(1);
    }

    GLuint default_vao;
    glGenVertexArrays(1, &default_vao);
    glBindVertexArray(default_vao);

    GLuint default_vbo;
    glGenBuffers(1, &default_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, default_vbo);
	glBufferData(GL_ARRAY_BUFFER, default_verts.size()*sizeof(float), default_verts.data(), GL_STATIC_DRAW);

    GLuint default_shader_id = LoadShaders("default_vert.glsl", "default_frag.glsl");
    GLint default_mvp_id = glGetUniformLocation(default_shader_id, "mvp");
    GLint default_posAttrib = glGetAttribLocation(default_shader_id, "position");
    GLint default_colAttrib = glGetAttribLocation(default_shader_id, "color");
    glEnableVertexAttribArray(default_posAttrib);
    glEnableVertexAttribArray(default_colAttrib);
    glVertexAttribPointer(default_posAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), 0);
    glVertexAttribPointer(default_colAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

    //
    // Setup the cube
    //

    GLfloat cube_verts[] = {
        // positions
        0.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 1.0f,1.0f,0.0f, 0.0f,1.0f,0.0f, //front
        0.0f,0.0f,1.0f, 1.0f,0.0f,1.0f, 1.0f,1.0f,1.0f, 0.0f,1.0f,1.0f, //back
        0.0f,0.0f,0.0f, 0.0f,0.0f,1.0f, 0.0f,1.0f,1.0f, 0.0f,1.0f,0.0f, //left
        1.0f,0.0f,0.0f, 1.0f,0.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,0.0f, //right
        0.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,0.0f,1.0f, //bottom
        0.0f,1.0f,0.0f, 1.0f,1.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,1.0f,1.0f, //top
        // normals
         0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f,
         0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f
    };

    GLuint cube_vao;
    glGenVertexArrays(1, &cube_vao);
    glBindVertexArray(cube_vao);

    GLuint cube_vbo;
    glGenBuffers(1, &cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW);

    GLuint cube_shader_id = LoadShaders("cube_vert.glsl", "cube_frag.glsl");
    GLint cube_mvp_id = glGetUniformLocation(cube_shader_id, "mvp");
    GLint cube_color_id = glGetUniformLocation(cube_shader_id, "color");
    GLint cube_posAttrib = glGetAttribLocation(cube_shader_id, "position");
    GLint cube_norAttrib = glGetAttribLocation(cube_shader_id, "normal");
    glEnableVertexAttribArray(cube_posAttrib);
    glEnableVertexAttribArray(cube_norAttrib);
    glVertexAttribPointer(cube_posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(cube_norAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3*4*6*sizeof(GLfloat)));


    // scale the unit cube to create all the parts
    glm::mat4 cam_hing    = glm::scale(glm::mat4(1.0f), glm::vec3( 1.0f,  1.0f,  1.0f));
    glm::mat4 cam_body    = glm::scale(glm::mat4(1.0f), glm::vec3( 3.0f,  6.0f,  4.0f));
    glm::mat4 cam_lens    = glm::scale(glm::mat4(1.0f), glm::vec3( 2.0f,  1.0f,  2.0f));
    glm::mat4 shaft_hing  = glm::scale(glm::mat4(1.0f), glm::vec3( 1.0f,  2.0f,  1.0f));
    glm::mat4 shaft_body  = glm::scale(glm::mat4(1.0f), glm::vec3( 2.0f, 20.0f,  2.0f));
    glm::mat4 base_hing   = glm::scale(glm::mat4(1.0f), glm::vec3( 6.0f,  3.0f,  6.0f));
    glm::mat4 base_body   = glm::scale(glm::mat4(1.0f), glm::vec3(12.0f,  2.0f, 20.0f));
    glm::mat4 desk_top    = glm::scale(glm::mat4(1.0f), glm::vec3(30.0f,  2.0f, 48.0f));
    glm::mat4 desk_leg_1  = glm::scale(glm::mat4(1.0f), glm::vec3( 4.0f, 28.0f,  4.0f));
    glm::mat4 desk_leg_2  = glm::scale(glm::mat4(1.0f), glm::vec3( 4.0f, 28.0f,  4.0f));
    glm::mat4 desk_leg_3  = glm::scale(glm::mat4(1.0f), glm::vec3( 4.0f, 28.0f,  4.0f));
    glm::mat4 desk_leg_4  = glm::scale(glm::mat4(1.0f), glm::vec3( 4.0f, 28.0f,  4.0f));

    // define the translations to assemble each part in their local object coordinate system
    glm::mat4 cam_hing_mcs   = glm::translate(glm::mat4(1.0f), glm::vec3( 0.0f,  0.0f,  0.0f));
    glm::mat4 cam_body_mcs   = glm::translate(glm::mat4(1.0f), glm::vec3( 1.0f, -3.0f, -2.0f));
    glm::mat4 cam_lens_mcs   = glm::translate(glm::mat4(1.0f), glm::vec3( 1.5f, -4.0f,  0.0f));
    glm::mat4 shaft_hing_scs = glm::translate(glm::mat4(1.0f), glm::vec3( 2.0f, 15.0f,  0.5f));
    glm::mat4 shaft_body_scs = glm::translate(glm::mat4(1.0f), glm::vec3( 0.0f,  0.0f,  0.0f));
    glm::mat4 base_body_bcs  = glm::translate(glm::mat4(1.0f), glm::vec3( 0.0f,  0.0f,  0.0f));
    glm::mat4 base_hing_bcs  = glm::translate(glm::mat4(1.0f), glm::vec3( 3.0f,  2.0f,  2.0f));
    glm::mat4 desk_top_dcs   = glm::translate(glm::mat4(1.0f), glm::vec3( 0.0f, 28.0f,  0.0f));
    glm::mat4 desk_leg_1_dcs = glm::translate(glm::mat4(1.0f), glm::vec3( 0.0f,  0.0f,  0.0f));
    glm::mat4 desk_leg_2_dcs = glm::translate(glm::mat4(1.0f), glm::vec3(26.0f,  0.0f,  0.0f));
    glm::mat4 desk_leg_3_dcs = glm::translate(glm::mat4(1.0f), glm::vec3( 0.0f,  0.0f, 44.0f));
    glm::mat4 desk_leg_4_dcs = glm::translate(glm::mat4(1.0f), glm::vec3(26.0f,  0.0f, 44.0f));

    // define the translations to assemble the different systems into each other
    glm::mat4 wcs     = glm::mat4(1.0f);
    glm::mat4 rcs_wcs = glm::translate(wcs,     glm::vec3(0.0f,  0.0f, 0.0f));
    glm::mat4 dcs_wcs = glm::translate(rcs_wcs, glm::vec3(0.0f,  0.0f, 0.0f));
    glm::mat4 ocs_wcs = glm::translate(dcs_wcs, glm::vec3(9.0f, 30.0f, 16.0f));

    // place the desk in world space
    glm::mat4 desk_top_wcs   = dcs_wcs * desk_top_dcs;
    glm::mat4 desk_leg_1_wcs = dcs_wcs * desk_leg_1_dcs;
    glm::mat4 desk_leg_2_wcs = dcs_wcs * desk_leg_2_dcs;
    glm::mat4 desk_leg_3_wcs = dcs_wcs * desk_leg_3_dcs;
    glm::mat4 desk_leg_4_wcs = dcs_wcs * desk_leg_4_dcs;

    // define the translations to assemble each component group into the OCS
    glm::mat4 base_ocs = glm::mat4(1.0f);
    glm::mat4 base_hing_ocs = base_ocs * base_hing_bcs;
    glm::mat4 base_body_ocs = base_ocs * base_body_bcs;

    glm::mat4 shaft_ocs = glm::translate(base_hing_ocs, glm::vec3(2.0f, 0.0f, 2.0f));
    glm::mat4 shaft_hing_ocs = shaft_ocs * shaft_hing_scs;
    glm::mat4 shaft_body_ocs = shaft_ocs * shaft_body_scs;

    glm::mat4 cam_ocs = shaft_hing_ocs;
    glm::mat4 cam_lens_ocs = cam_ocs * cam_lens_mcs;
    glm::mat4 cam_body_ocs = cam_ocs * cam_body_mcs;
    glm::mat4 cam_hing_ocs = cam_ocs * cam_hing_mcs;

    // define the translations to assemble the OCS in world space (WCS)
    glm::mat4 base_hing_wcs  = ocs_wcs * base_hing_ocs;
    glm::mat4 base_body_wcs  = ocs_wcs * base_body_ocs;
    glm::mat4 shaft_hing_wcs = ocs_wcs * shaft_hing_ocs;
    glm::mat4 shaft_body_wcs = ocs_wcs * shaft_body_ocs;
    glm::mat4 cam_lens_wcs   = ocs_wcs * cam_lens_ocs;
    glm::mat4 cam_body_wcs   = ocs_wcs * cam_body_ocs;
    glm::mat4 cam_hing_wcs   = ocs_wcs * cam_hing_ocs;


    //
    // Setup the projection and view matrices
    //

    // simple camera
    float camera_dist = 80.0f;
    glm::vec3 camera(camera_dist, 0.0f, 0.0f);

	//glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	glm::mat4 projection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 1000.0f);

    glm::mat4 view = glm::lookAt(
            camera,     // position
            glm::vec3(0,0,0),       // target
            glm::vec3(0,1,0)        // up
            );

    glm::mat4 mvp = projection * view * glm::mat4(1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    printf("Starting...\n");
    while (window.isOpen())
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
                    case sf::Keyboard::Left:
                        camera = glm::rotateY(camera, 45.0f);
                        view = glm::rotate(view, 45.0f, glm::vec3(0,1,0));
                        break;
                    case sf::Keyboard::Right:
                        camera = glm::rotateY(camera, -45.0f);
                        view = glm::rotate(view, -45.0f, glm::vec3(0,1,0));
                        break;
                    case sf::Keyboard::Up:
                        camera = glm::rotate(camera, 45.0f, glm::vec3(1,0,1));
                        break;
                    case sf::Keyboard::Down:
                        camera = glm::rotate(camera, -45.0f, glm::vec3(1,0,1));
                        break;
                    case sf::Keyboard::PageUp:
                        camera *= 0.8f;
                        break;
                    case sf::Keyboard::PageDown:
                        camera *= 1.8f;
                        break;
                }
                printf("Camera: %f %f %f\n", camera.x, camera.y, camera.z);
            }
        }
/*
        view = glm::lookAt(
                camera - glm::vec3(15.0f, 0.0f, 24.0f), // position
                glm::vec3(15.0f,0,24.0f),       // target
                glm::vec3(0,1,0)        // up
                );
                */

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.8, 0.8, 0.8, 0.0);

        //
        // Draw the body
        //
        glm::vec3 color;

		glUseProgram(cube_shader_id);

        glBindVertexArray(cube_vao);
        glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);

        // camera lense
        mvp = projection * view * cam_lens_wcs * cam_lens;
        color = glm::vec3(1.0f, 0.0f, 0.0f);
        glUniformMatrix4fv(cube_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(cube_color_id, color.x, color.y, color.z);
        glDrawArrays(GL_QUADS, 0, 24);

        // camera body
        mvp = projection * view * cam_body_wcs * cam_body;
        color = glm::vec3(1.0f, 1.0f, 0.0f);
        glUniformMatrix4fv(cube_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(cube_color_id, color.x, color.y, color.z);
        glDrawArrays(GL_QUADS, 0, 24);

        // camera hinge
        mvp = projection * view * cam_hing_wcs * cam_hing;
        color = glm::vec3(1.0f, 0.0f, 1.0f);
        glUniformMatrix4fv(cube_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(cube_color_id, color.x, color.y, color.z);
        glDrawArrays(GL_QUADS, 0, 24);

        // shaft hing
        mvp = projection * view * shaft_hing_wcs * shaft_hing;
        color = glm::vec3(0.0f, 0.0f, 1.0f);
        glUniformMatrix4fv(cube_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(cube_color_id, color.x, color.y, color.z);
        glDrawArrays(GL_QUADS, 0, 24);

        // shaft body
        mvp = projection * view * shaft_body_wcs * shaft_body;
        color = glm::vec3(0.0f, 1.0f, 1.0f);
        glUniformMatrix4fv(cube_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(cube_color_id, color.x, color.y, color.z);
        glDrawArrays(GL_QUADS, 0, 24);

        // baseboard hing
        mvp = projection * view * base_hing_wcs * base_hing;
        color = glm::vec3(0.5f, 1.0f, 1.0f);
        glUniformMatrix4fv(cube_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(cube_color_id, color.x, color.y, color.z);
        glDrawArrays(GL_QUADS, 0, 24);

        // baseboard body
        mvp = projection * view * base_body_wcs * base_body;
        color = glm::vec3(0.5f, 1.0f, 0.25f);
        glUniformMatrix4fv(cube_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(cube_color_id, color.x, color.y, color.z);
        glDrawArrays(GL_QUADS, 0, 24);

        // desk top
        mvp = projection * view * desk_top_wcs * desk_top;
        color = glm::vec3(0.5f, 0.25f, 1.0f);
        glUniformMatrix4fv(cube_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(cube_color_id, color.x, color.y, color.z);
        glDrawArrays(GL_QUADS, 0, 24);

        // desk leg 1
        mvp = projection * view * desk_leg_1_wcs * desk_leg_1;
        color = glm::vec3(0.5f, 0.25f, 1.0f);
        glUniformMatrix4fv(cube_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(cube_color_id, color.x, color.y, color.z);
        glDrawArrays(GL_QUADS, 0, 24);

        // desk leg 2
        mvp = projection * view * desk_leg_2_wcs * desk_leg_2;
        color = glm::vec3(0.5f, 0.25f, 1.0f);
        glUniformMatrix4fv(cube_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(cube_color_id, color.x, color.y, color.z);
        glDrawArrays(GL_QUADS, 0, 24);

        // desk leg 3
        mvp = projection * view * desk_leg_3_wcs * desk_leg_3;
        color = glm::vec3(0.5f, 0.25f, 1.0f);
        glUniformMatrix4fv(cube_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(cube_color_id, color.x, color.y, color.z);
        glDrawArrays(GL_QUADS, 0, 24);

        // desk leg 4
        mvp = projection * view * desk_leg_4_wcs * desk_leg_4;
        color = glm::vec3(0.5f, 0.25f, 1.0f);
        glUniformMatrix4fv(cube_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform3f(cube_color_id, color.x, color.y, color.z);
        glDrawArrays(GL_QUADS, 0, 24);

        //
        // Draw the grid
        //

        glUseProgram(default_shader_id);
        glBindVertexArray(default_vao);
        glBindBuffer(GL_ARRAY_BUFFER, default_vbo);

        mvp = projection * view * glm::mat4(1.0f);
        glUniformMatrix4fv(default_mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));

        glDrawArrays(GL_LINES, 0, default_verts.size()/6);

        window.display();
    }

    return 0;
}

