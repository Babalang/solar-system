#version 330 core

layout(location = 0) in vec3 vertices_position_modelspace;
layout(location = 1) in vec3 vertices_normals_modelspace;
layout(location = 2) in vec2 vertices_uv_modelspace;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform bool isSun;

out vec3 fragPosWorld; // Position du fragment dans l'espace monde
out vec3 normalInterp; // Normale interpolée à envoyer au fragment shader
out vec2 fragUV;

void main() {
    gl_Position = Projection * View * Model * vec4(vertices_position_modelspace, 1.0);
    fragPosWorld = vec3(Model * vec4(vertices_position_modelspace, 1.0));

    // Calcul des normales dans l'espace monde
    mat3 normalMatrix = transpose(inverse(mat3(Model)));
    normalInterp = normalize(normalMatrix * vertices_normals_modelspace);

    fragUV = vertices_uv_modelspace;
}
