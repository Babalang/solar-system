#version 330 core

// Input from the vertex shader
in vec3 fragPosition;
in vec3 fragNormal;

// Output color
out vec3 color;

// Uniforms
uniform vec3 lightPos;   // Position de la source lumineuse (ex: Soleil)
uniform vec3 lightColor; // Couleur de la lumière (ex: Soleil légèrement jaune)
uniform vec3 viewPos;    // Position de la caméra
uniform vec3 objectColor; // Couleur de l'objet (planète, astéroïde)

// Paramètres de lumière
uniform float ambientStrength = 0.1;
uniform float specularStrength = 0.5;
uniform int shininess = 32;

void main() {
    // Normalisation des vecteurs
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    
    // Composante diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Composante ambiante
    vec3 ambient = ambientStrength * lightColor;

    // Composante spéculaire (modèle de Phong)
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // Calcul final de la couleur
    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = result;
}
