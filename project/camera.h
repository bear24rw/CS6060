#pragma once

#include <glm/glm.hpp>

class Camera
{
    public:
        Camera();
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 right;
        glm::vec3 up;
        glm::mat4 proj_mat(void);
        glm::mat4 view_mat(void);
        void translate(glm::vec3);
        void increase_angle_v(float);
        void increase_angle_h(float);
        void set_size(int, int);
        float angle_horz;
        float angle_vert;
        float fov;
    private:
        void update_vectors(void);
        int _width;
        int _height;
};
