#version 410

in vec4 position_model;
in vec3 normal_model;

out Data{
    vec3 normal;
    vec4 position;
} vdata;

void main(){
    vdata.position = position_model;
    vdata.normal = normal_model;
}

