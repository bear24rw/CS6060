#version 150

in vec3 frag_pos;
in vec3 Normal;
uniform vec3 color;
out vec3 out_color;

void main()
{
    vec3 light_dir = vec3(0.3,0.9,0.1);
    float intensity = dot(light_dir, normalize(Normal));

    //vec3 tmp = vec3(fract(edge.x),fract(edge.y),fract(edge.z));
    /*
    float fr = edge.x - floor(edge.x);
    if (fr < 0.2) {
        out_color = vec3(0);
    } else {
        out_color = color;
    }
    */

//    if (any(greaterThanEqual(fract(frag_pos), vec3(0.2))) &&
        //any(lessThanEqual(fract(frag_pos), vec3(0.8)))) {
    //if (any(greaterThan(frag_pos.xy, vec2(1)))) {
    if ((fract(frag_pos.x) < 0.1 || fract(frag_pos.x) > 0.9) && (fract(frag_pos.y) < 0.2 || fract(frag_pos.y) > 0.9) ||
        (fract(frag_pos.x) < 0.1 || fract(frag_pos.x) > 0.9) && (fract(frag_pos.z) < 0.2 || fract(frag_pos.z) > 0.9) ||
        (fract(frag_pos.z) < 0.1 || fract(frag_pos.z) > 0.9) && (fract(frag_pos.y) < 0.2 || fract(frag_pos.y) > 0.9)) {
        out_color = vec3(0);//intensity * 0.1;
    } else {
        out_color = color;
    }
    //out_color = color * intensity;
}
