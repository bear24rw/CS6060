#version 150

in vec3 position;
in vec3 color;

uniform mat4 mvp;

out vec3 Color;

void main()
{
    Color = color;
    gl_Position = mvp * vec4(position, 1);
}
