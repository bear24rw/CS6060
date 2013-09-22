#version 150

in vec3 position_model;
in vec3 normal_model;

uniform mat4 MVP;

out vec3 norm;

void main()
{
    norm = normal_model;
    gl_Position = MVP * vec4(position_model, 1);
}
