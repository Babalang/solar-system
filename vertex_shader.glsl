#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;
layout(location = 1) in vec3 vertex_normal_modelspace;  // Ajout des normales

// Uniform transformations matrices Model View Projection
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for light properties
uniform vec3 lightPos;   // Position de la lumière (Soleil)
uniform vec3 lightColor; // Couleur de la lumière (ex. jaune pour le Soleil)
uniform vec3 viewPos;    // Position de la caméra (pour les calculs d'éclairage)

// Material properties
uniform vec3 Color_Vec;  // Couleur de l'objet

out vec3 o_color; // Couleur calculée pour chaque fragment

void main() {
    // Output position of the vertex in clip space (MVP transformation)
    gl_Position = Projection * View * Model * vec4(vertices_position_modelspace, 1.0);

    // Transformation de la normale dans l'espace monde
    vec3 fragPos = vec3(Model * vec4(vertices_position_modelspace, 1.0)); 
    vec3 norm = normalize(mat3(transpose(inverse(Model))) * vertex_normal_modelspace);  // Normales transformées

    // Calcul de la lumière directionnelle
    vec3 lightDir = normalize(lightPos - fragPos);  // Direction de la lumière
    float diff = max(dot(norm, lightDir), 0.0);     // Calcul de la lumière diffuse

    // Calcul de la couleur finale en fonction de la lumière et du matériau
    vec3 diffuse = diff * lightColor;               // Lumière diffuse basée sur l'angle de la normale
    vec3 ambient = 0.1 * lightColor;                // Lumière ambiante faible
    vec3 finalColor = (ambient + diffuse) * Color_Vec; // Combinaison lumière et couleur

    // Sortie de la couleur pour le fragment shader
    o_color = finalColor;
}
