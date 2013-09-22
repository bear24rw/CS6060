#version 150

in vec3 position_model;
in vec3 normal_model;
in vec2 texcoord;

uniform mat4 MVP;

out vec3 norm;
out vec2 Texcoord;

void main()
{
    Texcoord = texcoord;
    norm = normal_model;
    gl_Position = MVP * vec4(position_model, 1);
}
