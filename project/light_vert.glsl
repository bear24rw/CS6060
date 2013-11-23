#version 330 core
in vec3 position_model;
in vec3 normal_model;
in vec3 color;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 light_pos_world;

out vec3 Color;
out vec3 position_world;
out vec3 normal_cam;
out vec3 eye_dir_cam;
out vec3 light_dir_cam;

void main() {
   Color = color;

   // output position of the vertex
   gl_Position = MVP*vec4(position_model,1);

   // position of the vertex in world space
   position_world = (M*vec4(position_model,1)).xyz;

   // vector that goes from vertex to the camera in camera space
   // in camera space the camera is at 0,0,0
   vec3 position_cam = (V*M*vec4(position_model,1)).xyz;
   eye_dir_cam = vec3(0,0,0) - position_cam;

   // vector that goes from the vertex to the light
   vec3 light_pos_cam = (V * vec4(light_pos_world,1)).xyz;
   light_dir_cam = light_pos_cam + eye_dir_cam;

   // normal of the vertex in camera space
   normal_cam = (V*M*vec4(normal_model,0)).xyz;
}
