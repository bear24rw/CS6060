#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

Camera::Camera()
{
    distance = 50.0f;
    target = glm::vec3(0.0f);
    angle_horz = 45.0f;
    angle_vert = 20.0f;
    fov = 45.0f;
    _width = 800;
    _height = 600;
    update_vectors();
}

void Camera::increase_angle_v(float angle)
{
    angle_vert += angle;
    update_vectors();
}

void Camera::increase_angle_h(float angle)
{
    angle_horz += angle;
    update_vectors();
}

void Camera::increase_distance(float dist)
{
    distance += dist;
    update_vectors();
}

void Camera::update_vectors()
{
    direction = glm::vec3(
            cos(angle_vert) * sin(angle_horz),
            sin(angle_vert),
            cos(angle_vert) * cos(angle_horz)
            );

    right = glm::vec3(
            sin(angle_horz - 3.14f/2.0f),
            0,
            cos(angle_horz - 3.14f/2.0f)
            );

    up = glm::cross(right, direction);

    position = distance * direction;
}

glm::mat4 Camera::view_mat(void)
{
    return glm::lookAt(
            target + (distance * direction),
            target,
            up);
}

glm::mat4 Camera::proj_mat(void)
{
    return glm::perspective(fov, (float)_width/(float)_height, 0.01f, 10000.0f);
}

void Camera::set_size(int width, int height)
{
    _width = width;
    _height = height;
}
