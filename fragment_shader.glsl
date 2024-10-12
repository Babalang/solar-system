#version 330 core

// Ouput data
out vec3 color;

in vec3 o_color;

void main(){

        color =vec3(0.2, 0.2,0.4)+o_color;

}
