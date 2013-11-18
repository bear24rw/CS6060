#version 150

in vec3 frag_pos;
in vec3 Normal;
uniform vec3 color;
out vec3 out_color;

void main()
{
    vec3 light_dir = vec3(-0.3,0.9,0.1);
    float intensity = dot(light_dir, normalize(Normal));

    vec3 f = fract(frag_pos);

    if ((f.x < 0.1 || f.x > 0.9) && (f.y < 0.1 || f.y > 0.9) ||
        (f.x < 0.1 || f.x > 0.9) && (f.z < 0.1 || f.z > 0.9) ||
        (f.z < 0.1 || f.z > 0.9) && (f.y < 0.1 || f.y > 0.9)) {
        out_color = vec3(0);
    } else {
        out_color = color;
    }

    //out_color = color * intensity;
}
