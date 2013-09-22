#version 330

in vec3 tex_coord;
uniform samplerCube cubemap;
out vec4 color;

void main()
{
    color = texture(cubemap, tex_coord);
}
