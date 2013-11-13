#version 330 core

in vec3 position;
in vec3 normal;
uniform mat4 mvp;
out vec3 Normal;
out vec3 edge;

void main(){
    edge = position;
    Normal = normal;
	gl_Position =  mvp * vec4(position,1);
}

