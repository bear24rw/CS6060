#version 150

in vec3 position_model;
in vec3 normal_model;
in vec2 texcoord;
in vec3 color;

uniform mat4 MVP;

out vec3 norm;
out vec2 Texcoord;
out vec3 vert_color;

void main()
{
    vert_color = color;
    Texcoord = texcoord;
    norm = normal_model;
    gl_Position = MVP * vec4(position_model, 1);
}
