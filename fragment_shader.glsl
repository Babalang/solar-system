#version 330 core

in vec3 fragPosWorld; // Position du fragment interpolée
in vec3 normalInterp; // Normale interpolée
in vec2 fragUV;

uniform sampler2D Texture;
uniform vec3 lightPos;
uniform vec3 ambientLight;
uniform vec3 diffuseLight;
uniform vec3 specular;
uniform vec3 camera_position;
uniform int shininess;
uniform bool isSun;

out vec3 o_color;

void main() {
        // Lire la couleur de la texture pour la Terre
    vec3 texColor = texture(Texture, fragUV).rgb;

    if (isSun) {
        o_color = texColor * 2.0; // Le Soleil est simplement coloré
    } else {
        // Normaliser la normale interpolée
        vec3 normal = normalize(normalInterp);

        // Vecteur lumière
        vec3 lightDir = normalize(lightPos - fragPosWorld);

        // Lumière ambiante
        float Ka = 0.2;
        vec3 I_ambient = ambientLight * Ka * texColor;

        // Lumière diffuse
        float Kd = 0.5;
        vec3 I_diffuse = diffuseLight * Kd * max(dot(normal, lightDir), 0.0) * texColor;

        // Lumière spéculaire
        vec3 viewDir = normalize(camera_position - fragPosWorld);
        vec3 R = reflect(-lightDir, normal);
        float Ks = 0.2;
        vec3 I_specular = specular * Ks * pow(max(dot(R, viewDir), 0.0), shininess);

        // Couleur finale
        o_color = clamp(I_ambient + I_diffuse + I_specular, 0.0, 1.0);
;
    }
}
