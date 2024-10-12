#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;

//TODO create uniform transformations matrices Model View Projection
// Values that stay constant for the whole mesh.
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 Color_Vec;
out vec3 o_color;

void main(){

        // TODO : Output position of the vertex, in clip space : MVP * position
        gl_Position = (Projection*View*Model)*vec4(vertices_position_modelspace,1);

        o_color = vec3(0.,0.,0.) + Color_Vec;

}

