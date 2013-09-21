#version 330 core

// interpolated values from the vertex shader
in vec3 Color;
in vec3 position_world;
in vec3 normal_cam;
in vec3 eye_dir_cam;
in vec3 light_dir_cam;

out vec3 outColor;

uniform mat4 MV;
uniform vec3 light_pos_world;

void main() {

    // light emission properties
    vec3 light_color = vec3(1,1,1);
    float light_power = 200.0f;

    // material properties
    vec3 material_diffuse_color = Color;
    vec3 material_ambient_color = vec3(0.1,0.1,0.1) * material_diffuse_color;
    vec3 material_specular_color = vec3(0.3,0.3,0.3);

    // distance to the light
    float distance = length(light_pos_world - position_world);

    // normal of the computed fragment in camera space
    vec3 n = normalize(normal_cam);
    // direction of the light (from fragment to the light)
    vec3 l = normalize(light_dir_cam);
    // cosine of the angle between the normal and the light direction
    // clamed above 0
    // - light is at the vertical of the triangle -> 1
    // - light is perpendicular to the triangle -> 0
    // - light is behind the triangle -> 0
    float cosTheta = clamp(dot(n,l), 0, 1);

    // eye vector towards the camera
    vec3 E = normalize(eye_dir_cam);
    // direction in which the triangle reflects the light
    vec3 R = reflect(-l,n);
    // cosine of the angle between the eye vector and the reflect vector
    // clamped to 0
    // - looking into the reflection -> 1
    // - looking elsewhere -> < 1
    float cosAlpha = clamp(dot(E,R), 0, 1);

    outColor =
        // ambient: simulates indirect lighting
        material_ambient_color +
        // diffuse: 'color' of the object
        material_diffuse_color * light_color * light_power * cosTheta / (distance*distance) +
        // specular: reflective highlights, like a mirror
        material_specular_color * light_color * light_power * pow(cosAlpha,5) / (distance*distance);

}
