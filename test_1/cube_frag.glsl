#version 150

in vec3 edge;
in vec3 Normal;
uniform vec3 color;
out vec3 out_color;

void main()
{
    vec3 light_dir = vec3(0.3,0.9,0.1);
    float intensity = dot(light_dir, normalize(Normal));

    out_color = color * intensity;
}
