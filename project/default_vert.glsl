#version 150

in vec3 position_model;
in vec3 color;

uniform mat4 MVP;

out vec3 Color;

void main()
{
    Color = color;
    gl_Position = MVP * vec4(position_model, 1);
}
