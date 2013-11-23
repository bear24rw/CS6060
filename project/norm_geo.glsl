#version 410
#extension GL_EXT_geometry_shader4 : enable

layout(triangles) in;
layout(line_strip, max_vertices = 3) out;

uniform mat4 MVP;

in Data{
    vec3 normal;
    vec4 position;
} vdata[];

out Data{
    vec3 color;
} gdata;

void main(){
    /*
    vec4 middle = (vdata[0].position + vdata[1].position + vdata[2].position)/3;
    middle.w = 1.0;
    vec3 normal = normalize((vdata[0].normal + vdata[1].normal + vdata[2].normal)/3);
    */

    gl_Position = MVP * vdata[0].position;
    gdata.color = vec3(0);
    EmitVertex();

    gl_Position = MVP * (vdata[0].position + vec4(vdata[0].normal*0.4, 0));
    gdata.color = vdata[0].normal*0.5+0.5;
    EmitVertex();

    EndPrimitive();
}
