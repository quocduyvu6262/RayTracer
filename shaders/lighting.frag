#version 330 core

in vec4 position; // raw position in the model coord
in vec3 normal;   // raw normal in the model coord

uniform mat4 modelview; // from model coord to eye coord
uniform mat4 view;      // from world coord to eye coord

// Material parameters
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emision;
uniform float shininess;

// Light source parameters
const int maximal_allowed_lights = 10;
uniform bool enablelighting;
uniform int nlights;
uniform vec4 lightpositions[ maximal_allowed_lights ];
uniform vec4 lightcolors[ maximal_allowed_lights ];

// Output the frag color
out vec4 fragColor;


void main (void){
    if (!enablelighting){
        // Default normal coloring (you don't need to modify anything here)
        vec3 N = normalize(normal);
        fragColor = vec4(0.5f*N + 0.5f , 1.0f);
    } else {
        
        // HW3: You will compute the lighting here.
        vec3 normal_cam = normalize(inverse(transpose(mat3(modelview))) * normal);

        vec4 position_cam = modelview * position;
        vec3 v = normalize(vec3(0,0,0) - vec3(position_cam.x,position_cam.y,position_cam.z));
        vec4 sum = vec4(0,0,0,0);
        for(int i=0; i<3; i++){
            vec4 lightposition_cam = view * lightpositions[i];
            vec3 lightdirection = normalize(vec3(position_cam.w * lightposition_cam.x, position_cam.w * lightposition_cam.y, position_cam.w * lightposition_cam.z) - 
                                            vec3(lightposition_cam.w * position_cam.x, lightposition_cam.w * position_cam.y, lightposition_cam.w * position_cam.z));
            vec3 halfwaydirection = normalize(v + lightdirection);
            sum += lightcolors[i]*(ambient + diffuse*max(dot(normal_cam, lightdirection), 0) + 
                    specular*pow(max(dot(normal_cam, halfwaydirection),0), shininess));
        }
        fragColor = emision + sum;
    }
}
