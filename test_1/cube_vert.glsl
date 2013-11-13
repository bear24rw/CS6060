#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec3 position;
in vec3 normal;

out vec3 Normal;
out vec3 frag_pos;

void main(){
    frag_pos = vec3(model * vec4(position, 0)).zxy;
    Normal = normal;
	gl_Position = proj * view * model * vec4(position,1);
}

