#version 330

in vec3 position_model;
uniform mat4 mvp;
out vec3 tex_coord;

void main()
{
    gl_Position = mvp * vec4(position_model, 1);
    tex_coord = position_model;
}
