#version 330 core

// Interpolated values from the fragex shaders
in vec2 UV;
in vec3 vert_pos_world;
in vec3 vert_nor_cam;
in vec3 eye_dir_cam;
in vec3 light_dir_cam;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D tex;
uniform mat4 MV;
uniform vec3 light_pos_world;

void main(){

	// Light emission properties
	vec3 light_color = vec3(1,1,1);
	float light_power = 1500.0f;

	// Material properties
	vec3 diffuse_color = texture2D(tex, UV ).rgb;
	vec3 ambient_color = vec3(0.1,0.1,0.1) * diffuse_color;
	vec3 specular_color = vec3(0.3,0.3,0.3);

	// Distance to the light
	float distance = length(light_pos_world - vert_pos_world );

	vec3 n = normalize( vert_nor_cam );
	vec3 l = normalize( light_dir_cam );
	float cosTheta = clamp( dot( n,l ), 0,1 );

	vec3 E = normalize(eye_dir_cam);
	vec3 R = reflect(-l,n);
	float cosAlpha = clamp( dot( E,R ), 0,1 );

	color = ambient_color +
            diffuse_color * light_color * light_power * cosTheta / (distance*distance) +
            specular_color * light_color * light_power * pow(cosAlpha,5) / (distance*distance);

}
