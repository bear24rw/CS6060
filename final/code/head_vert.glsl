#version 330 core

// Input vertex data, different for all executions of this shader.
in vec3 vert_pos_model;
in vec3 vert_nor_model;
in vec2 vert_tex;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 vert_pos_world;
out vec3 vert_nor_cam;
out vec3 eye_dir_cam;
out vec3 light_dir_cam;

// Values that stay constant for the whole mesh.
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 light_pos_world;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  P * V * M * vec4(vert_pos_model,1);

	// Position of the vertex, in worldspace : M * position
	vert_pos_world = (M * vec4(vert_pos_model,1)).xyz;

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vert_pos_cam = ( V * M * vec4(vert_pos_model,1)).xyz;
	eye_dir_cam = vec3(0,0,0) - vert_pos_cam;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 light_pos_cam = ( V * vec4(light_pos_world,1)).xyz;
	light_dir_cam = light_pos_cam + eye_dir_cam;

	// Normal of the the vertex, in camera space
	vert_nor_cam = ( V * M * vec4(vert_nor_model,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

	// UV of the vertex. No special space for this one.
	UV = vert_tex;
}

