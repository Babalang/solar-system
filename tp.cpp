// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <random>

// Include GLEW
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "src/stb_image.h"


// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <GL/glut.h>

using namespace glm;
using Vec3 = glm::vec3;

#include "src/shader.hpp"
#include "src/objloader.hpp"
std::string currentFilename = "data/chair.off"; // Fichier par défaut

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 camera_position   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);

//booleans
bool affiche_asteroid  = false;

// timing
float deltaTime = 0.1f;	// time between current frame and last frame
float lastFrame = 0.0f;

//rotation
float angle = 0.;
float angleSol = 0.;
float angleTer = 250.;
float angleMoon = 130.;
float angleRotTer = 0.;
float angleMer =168.0f;
float angleRotMer = 0.0f;
float angleVen =65.0f;
float angleRotVen = 0.0f;
float angleMars = 49.0f;
float angleRotMars = 0.0f;
float angleJup = 350.0f;
float angleRotJup = 0.0f;
float angleSat = 229.0f;
float angleRotSat = 0.0f;
float angleUranus = 10.0f;
float angleRotUranus = 0.0f;
float angleNept = 310.0f;
float angleRotNept = 0.0f;
std::vector<float> langleast;
float zoom = 1;
int nbast = 18000;

//liste astres
std::vector<std::string> planets = {"Soleil","Terre", "Lune", "Mercure","Venus","Mars","Jupiter","Saturne","Uranus","Neptune"};

//translation
float tx =0.;
float ty =0.;


static GLint window;
static bool mouseRotatePressed = false;
static bool mouseMovePressed = false;
static bool mouseZoomPressed = false;
static int lastX=0, lastY=0, lastZoom=0;
static bool fullScreen = false;


GLuint programID;
GLuint VertexArrayID;
GLuint vertexbuffer;
GLuint normalbuffer;
GLuint elementbuffer;
GLuint uvbuffer;
GLuint LightID;
std::vector<GLuint> textures;


std::vector<unsigned short> indices; //Triangles concaténés dans une liste
std::vector<std::vector<unsigned short> > triangles;
std::vector<glm::vec3> indexed_vertices;
std::vector<glm::vec3> normals;
std::vector<glm::vec2> UV;


glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 0 );
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;


	// Right vector
glm::vec3 rightVector() {
    return glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
}

// Direction : Spherical coordinates to Cartesian coordinates conversion
glm::vec3 directionVector() {
    return glm::vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );
}

void computeMatricesFromInputs(float moveX, float moveY);
void initLight ();
void init ();
void draw ();
void display ();
void idle ();
void key (unsigned char keyPressed, int x, int y);
void mouse (int button, int state, int x, int y);
void motion (int x, int y);
void reshape(int w, int h);
int main (int argc, char ** argv);
void printMatrix(const glm::mat4& mat);

// ------------------------------------

void printMatrix(const glm::mat4& mat) {
    std::cout << mat[0][0] << " " << mat[1][0] << " " << mat[2][0] << " " << mat[3][0] << "\n" << mat[0][1] << " " << mat[1][1] << " " << mat[2][1] << " " << mat[3][1] << "\n" << mat[0][2] << " " << mat[1][2] << " " << mat[2][2] << " " << mat[3][2] << "\n" << mat[0][3] << " " << mat[1][3] << " " << mat[2][3] << " " << mat[3][3] << std::endl;
}

void initLight () {
    GLfloat light_position1[4] = {22.0f, 16.0f, 50.0f, 0.0f};
    GLfloat direction1[3] = {-52.0f,-16.0f,-50.0f};
    GLfloat color1[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat ambient[4] = {0.3f, 0.3f, 0.3f, 0.5f};

    glLightfv (GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, direction1);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, color1);
    glLightfv (GL_LIGHT1, GL_SPECULAR, color1);
    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, ambient);
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHTING);
}

void init () {
    // camera.resize (SCREENWIDTH, SCREENHEIGHT);
    initLight ();
    // glCullFace (GL_BACK);
    // glEnable (GL_CULL_FACE);
    glDepthFunc (GL_LESS);
    glEnable (GL_DEPTH_TEST);
    glClearColor (0.2f, 0.2f, 0.3f, 1.0f);
    glEnable(GL_COLOR_MATERIAL);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return;
    }

}
//Fonction pour charger un fichier .off différent :
void RechargeOFF(std::string name){
        currentFilename = name;
        indexed_vertices.clear();
        indices.clear();
        loadOFF(currentFilename, indexed_vertices, indices, triangles);
        std::cout << "Chargement de " << currentFilename << std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), indexed_vertices.data(), GL_STATIC_DRAW);
        std::cout<<indexed_vertices.size()<<std::endl;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);
}

GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Charger l'image avec stb_image
    int width, height, numChannels;
    unsigned char* data = stbi_load(path, &width, &height, &numChannels, 0);
    if (data) {
        // Définir les paramètres de texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // Définir les paramètres de texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    
    return textureID;
}

void loadAllTextures() {
    // Exemple de chemins vers les textures pour différentes planètes
    const char* texturePaths[planets.size()] = {
        "data/2k_sun.jpg",
        "data/2k_earth.jpg",
        "data/2k_moon.jpg",
        "data/2k_mercury.jpg",
        "data/2k_venus.jpg",
        "data/2k_mars.jpg",
        "data/2k_jupiter.jpg",
        "data/2k_saturn.jpg",
        "data/2k_uranus.jpg",
        "data/2k_neptune.jpg"
    };
    for (int i = 0; i < planets.size(); i++) {
        textures.push_back(loadTexture(texturePaths[i]));
    }
}

//Fonction pour afficher les différents astres :
mat4 affichePlanete(mat4 Model_matrix, int nbtexture, float revolution, float SunRotPerMinutes, float distance, float inclinaison, float NbRotation, float scale, GLuint loc, GLuint locColor,Vec3 ambientLight,Vec3 diffuseLight,Vec3 specular, int shininess,bool isSun=false){
    if(isSun) glUniform1i(glGetUniformLocation(programID,"isSun"),1);
    else glUniform1i(glGetUniformLocation(programID,"isSun"),0);
    glUniform3fv(glGetUniformLocation(programID, "ambientLight"), 1, &ambientLight[0]);
    glUniform3fv(glGetUniformLocation(programID, "specular"), 1, &specular[0]);
    glUniform3fv(glGetUniformLocation(programID, "diffuseLight"), 1, &diffuseLight[0]);
    glUniform1i(glGetUniformLocation(programID,"shininess"), shininess);
    glUniform3fv(glGetUniformLocation(programID,"camera_position"),1,&camera_position[0]);

    GLuint myTexture = textures[nbtexture];
    glActiveTexture(GL_TEXTURE0); // Activez l'unité de texture 0
    glBindTexture(GL_TEXTURE_2D, myTexture); // Liez votre texture
    glUniform1i(glGetUniformLocation(programID, "Texture"), 0); // 0 correspond à GL_TEXTURE0

    Model_matrix = glm::rotate(Model_matrix,glm::radians(revolution * SunRotPerMinutes),glm::vec3(0.0f,1.0f,0.0f));
    Model_matrix = glm::translate(Model_matrix,glm::vec3(distance,0.0f,0.0f));
    Model_matrix = glm::rotate(Model_matrix, glm::radians(inclinaison),glm::vec3(0.0f,0.0f,1.0f));
    mat4 Model_matrix1 = Model_matrix;
    Model_matrix = glm::rotate(Model_matrix, glm::radians(NbRotation * SunRotPerMinutes),glm::vec3(0.0f,1.0f,0.0f));
    Model_matrix = glm::scale(Model_matrix,glm::vec3(scale,scale,scale));
    glUniformMatrix4fv(loc,1,GL_FALSE, &Model_matrix[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    //std::cout<<"hello world"<<std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0,(void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, UV.size() * sizeof(glm::vec2), &UV[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_SHORT,(void*)0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    return Model_matrix1;
}

glm::vec3 calculateNormal(glm::vec3 v1) {
    glm::vec3 normal = glm::vec3(v1.x/sqrt(pow(v1.x,2)+pow(v1.y,2)+pow(v1.z,2)),v1.y/sqrt(pow(v1.x,2)+pow(v1.y,2)+pow(v1.z,2)),v1.z/sqrt(pow(v1.x,2)+pow(v1.y,2)+pow(v1.z,2)));
    return normal;
}
// ------------------------------------
// rendering.
// ------------------------------------
int currentTarget =0;
float SunRotPerMinutes = 1.0f;
void draw () {
    glUseProgram(programID);
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
    std::vector<mat4> lplanet;
    ProjectionMatrix = glm::perspective(45.0f,4.0f/3.0f,0.1f,10000.0f);
    GLuint locProj = glGetUniformLocation(programID, "Projection");
    glUniformMatrix4fv(locProj,1,GL_FALSE, &ProjectionMatrix[0][0]);
    mat4 Model_matrix_Sol(1.);
    GLuint loc = glGetUniformLocation(programID, "Model");
    GLuint locColor = glGetUniformLocation(programID, "Color_Vec");
    //Affichage du Soleil
    Model_matrix_Sol = affichePlanete(Model_matrix_Sol,0, 0.0f, SunRotPerMinutes, 0.0f, 0.0f, angleSol, 20.0f,loc,locColor,Vec3(1.0f, 1.0f, 0.5f),Vec3(1.0f, 1.0f, 0.5f),Vec3(1.0f, 1.0f, 0.8f),100,true);
    lplanet.push_back(Model_matrix_Sol);

    //Système solaire
        //Affichage de la Terre
        mat4 Model_matrix_Ter = Model_matrix_Sol;
        Model_matrix_Ter = affichePlanete(Model_matrix_Ter,1, angleTer, SunRotPerMinutes, 100.0f, 23.44f, angleRotTer, 1.0f,loc,locColor,Vec3(0.2, 0.2, 0.2),Vec3(0.5, 0.5, 1.0),Vec3(1.0, 1.0, 1.0),30);
        lplanet.push_back(Model_matrix_Ter);
        //Système planétaire
            //affichage de la Lune
            mat4 Model_matrix_Moon = Model_matrix_Ter;
            Model_matrix_Moon = glm::rotate(Model_matrix_Moon,glm::radians(-18.3f),glm::vec3(0.0f,0.0f,1.0f));
            Model_matrix_Moon = affichePlanete(Model_matrix_Moon,2, angleMoon, SunRotPerMinutes, 3.0f, 6.68f, angleMoon, 0.27f, loc,locColor,Vec3(0.1, 0.1, 0.1),Vec3(0.7, 0.7, 0.7),Vec3(0.5, 0.5, 0.5),20);
            lplanet.push_back(Model_matrix_Moon);
        //affichage de Mercure
        mat4 Model_matrix_Mer = Model_matrix_Sol;
        Model_matrix_Mer = affichePlanete(Model_matrix_Mer, 3, angleMer, SunRotPerMinutes, 39.0f, 0.03f, angleRotMer, 0.38f, loc,locColor,Vec3(0.2, 0.2, 0.2),Vec3(0.6, 0.6, 0.6),Vec3(1.0, 1.0, 1.0),10);
        lplanet.push_back(Model_matrix_Mer);
        //Affichage de Venus
        mat4 Model_matrix_Ven = Model_matrix_Sol;
        Model_matrix_Ven = affichePlanete(Model_matrix_Ven, 4, angleVen, SunRotPerMinutes, 72.0f, 177.4f, angleRotVen, 0.95f,loc,locColor,Vec3(0.2, 0.2, 0.2),Vec3(0.9, 0.8, 0.5),Vec3(1.0, 1.0, 1.0),20);
        lplanet.push_back(Model_matrix_Ven);
        //Affichage de Mars
        mat4 Model_matrix_Mars = Model_matrix_Sol;
        Model_matrix_Mars = affichePlanete(Model_matrix_Mars,5, angleMars, SunRotPerMinutes, 152.0f, 25.2f, angleRotMars, 0.53f,loc,locColor,Vec3(0.2, 0.2, 0.2),Vec3(0.8, 0.4, 0.4),Vec3(1.0, 1.0, 1.0),15);
        lplanet.push_back(Model_matrix_Mars);
        //Affichage de Jupiter
        mat4 Model_matrix_Jup = Model_matrix_Sol;
        Model_matrix_Jup = affichePlanete(Model_matrix_Jup, 6, angleJup, SunRotPerMinutes, 520.0f, 3.13f, angleRotJup, 11.2f,loc,locColor,Vec3(0.2, 0.2, 0.2),Vec3(0.6, 0.5, 0.3),Vec3(1.0, 1.0, 1.0),15);
        lplanet.push_back(Model_matrix_Jup);
        //Affichage de Saturne
        mat4 Model_matrix_Sat = Model_matrix_Sol;
        Model_matrix_Sat = affichePlanete(Model_matrix_Sat, 7, angleSat, SunRotPerMinutes, 958.0f, 27.0f, angleRotSat, 9.45f,loc,locColor,Vec3(0.2, 0.2, 0.2),Vec3(0.7, 0.7, 0.3),Vec3(1.0, 1.0, 1.0),15);
        lplanet.push_back(Model_matrix_Sat);
        //Affichage de Uranus
        mat4 Model_matrix_Uranus = Model_matrix_Sol;
        Model_matrix_Uranus = affichePlanete(Model_matrix_Uranus, 8, angleUranus, SunRotPerMinutes, 1920.0f, 97.8f, angleRotUranus, 4.0f,loc,locColor,Vec3(0.2, 0.2, 0.2),Vec3(0.6, 0.8, 0.9),Vec3(1.0, 1.0, 1.0),15);
        lplanet.push_back(Model_matrix_Uranus);
        //Affichage de Neptune
        mat4 Model_matrix_Nept = Model_matrix_Sol;
        Model_matrix_Nept = affichePlanete(Model_matrix_Nept,9, angleNept, SunRotPerMinutes, 3005.0f, 28.0f, angleRotNept, 3.88f,loc,locColor,Vec3(0.2, 0.2, 0.2),Vec3(0.5, 0.5, 0.7),Vec3(1.0, 1.0, 1.0),15);
        lplanet.push_back(Model_matrix_Nept);     
        //Affichage astéroides
        if(affiche_asteroid){
            float a =0.;
            for(int i =0;i<nbast/2;i++){
                if (i % 60 == 0 && i != 0) { // Assurez-vous de ne pas incrémenter a à 0
                    a++;
                }
                mat4 Model_matrix_Ast = Model_matrix_Sol;
                Model_matrix_Ast = affichePlanete(Model_matrix_Ast,2,langleast[i],SunRotPerMinutes,200.0f +(5.0f*a*(3600.0f/(float(nbast)/2))),0.0f,0.0f,0.5f,loc,locColor,Vec3(0.1, 0.1, 0.1),Vec3(0.5, 0.5, 0.5),Vec3(0.8, 0.8, 0.8),15);    
            }
            for(int i =0;i<nbast;i++){
                if (i % 60 == 0 && i != 0) { // Assurez-vous de ne pas incrémenter a à 0
                    a++;
                }
                mat4 Model_matrix_Ast = Model_matrix_Sol;
                Model_matrix_Ast = affichePlanete(Model_matrix_Ast,4,langleast[i],SunRotPerMinutes,3050.0f +(10.0f*a*(3600.0f/float(nbast))),0.0f,0.0f,0.6f,loc,locColor,Vec3(0.1, 0.1, 0.1),Vec3(0.8, 0.8, 0.8),Vec3(1.0, 1.0, 1.0),30);    
            }
        }
    float zom = zoom * 500;
    if(currentTarget < float(lplanet.size())){
        camera_position = (glm::vec3(lplanet[currentTarget][3]) + glm::vec3(15.0f+zom,15.0f+zom,0.0f));   
        camera_target = glm::vec3(lplanet[currentTarget][3]);    
    }   
    ViewMatrix = glm::lookAt(camera_position,camera_target,camera_up);
    GLuint locView = glGetUniformLocation(programID, "View");
    glUniformMatrix4fv(locView,1,GL_FALSE, &ViewMatrix[0][0]);
    Vec3 lightPos= { 0.0f, 0.0f, 0.0f};  // Position du soleil (par exemple, au centre du système)
    glUniform3fv(glGetUniformLocation(programID, "lightPos"), 1, &lightPos[0]);
}


void display () {
    glLoadIdentity ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // camera.apply ();
    draw ();
    glFlush ();
    glutSwapBuffers ();
}
static unsigned int FPS = 0;
//Fonction qui va calculer les coordonnées de chaque astres :
float speedFactor = 1.0f;
void idle () {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    static unsigned int counter = 0;
    static float lastTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    counter++;
    float currentTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    if (currentTime - lastTime >= 1000.0f) {
        FPS = counter;
        counter = 0;
        static char winTitle [64];
        sprintf (winTitle, "Raytracer - FPS: %d", FPS);
        glutSetWindowTitle (winTitle);
        lastTime = currentTime;
    }
    glutPostRedisplay ();
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;
    deltaTime = time - lastFrame;
    lastFrame = time;
    angleSol += deltaTime * 90.0f* speedFactor;
    if (angleSol > 360.0f) {
        angleSol -= 360.0f;
    }
    angleTer +=deltaTime * (360.0f/60.0f) * speedFactor;
    if(angleTer > 360.0f){
        angleTer -= 360.0f;
    }
    angleRotTer += deltaTime * angleTer * 365 * speedFactor;
    if(angleRotTer > 360.0f){
        angleRotTer -= 360.0f;
    }
    angleMoon += deltaTime * (81.0f) * speedFactor;
    if(angleMoon > 360.0f){
        angleMoon -= 360.0f;
    }
    angleRotMer += deltaTime * (37.0f) * speedFactor;
    if(angleRotMer > 360.0f){
        angleRotMer -= 360.0f;
    }
    angleMer += deltaTime * (25.0f) * speedFactor;
    if(angleMer > 360.0f){
        angleMer -= 360.0f;
    } 
    angleRotVen += deltaTime * (9.0f) * speedFactor;
    if(angleRotVen > 360.0f){
        angleRotVen -= 360.0f;
    }
    angleVen += deltaTime * (10.0f) * speedFactor;
    if(angleVen > 360.0f){
        angleVen -= 360.0f;
    }
    angleRotMars += deltaTime * (2180.0f) * speedFactor;
    if(angleRotMars > 360.0f){
        angleRotMars -= 360.0f;
    }
    angleMars += deltaTime * (3.25f) * speedFactor;
    if(angleMars > 360.0f){
        angleMars -= 360.0f;
    }
    angleJup += deltaTime * (0.5f) * speedFactor;
    if(angleJup > 360.0f) angleJup -=360.0f;
    angleRotJup += deltaTime * (5256.0f) * speedFactor;
    if(angleRotJup>360.0f){
        angleRotJup -=360.0f;
    }
    angleSat += deltaTime * (0.2f)*speedFactor;
    if(angleSat > 360.0f) angleSat -=360.0f;
    angleRotSat += deltaTime * (5005.7f) * speedFactor;
    if(angleRotSat>360.0f){
        angleRotSat -=360.0f;
    }
    angleUranus += deltaTime * (0.07f)*speedFactor;
    if(angleUranus > 360.0f) angleUranus -=360.0f;
    angleRotUranus += deltaTime * (3091.8f) * speedFactor;
    if(angleRotUranus >360.0f){
        angleRotUranus -=360.0f;
    }
    angleNept += deltaTime * (0.036f)*speedFactor;
    if(angleNept > 360.0f) angleNept -=360.0f;
    angleRotNept += deltaTime * (3285.0f) * speedFactor;
    if(angleRotNept >360.0f){
        angleRotNept -=360.0f;
    }
    if(affiche_asteroid){
        for(int i =0;i<nbast;i++){
            langleast[i] += deltaTime * (0.1f)*speedFactor;
            if(langleast[i]>360.0f) langleast[i]-=360.0f;
        }
    }

}

void key (unsigned char keyPressed, int x, int y) {
    float cameraSpeed = 2.5 * deltaTime;
    switch (keyPressed) {
    case 'f':
        if (fullScreen == true) {
            glutReshapeWindow (SCR_WIDTH, SCR_HEIGHT);
            fullScreen = false;
        } else {
            glutFullScreen ();
            fullScreen = true;
        }
        break;
    case 'a':
        affiche_asteroid = !affiche_asteroid;
        break;
    case 'q':
        nbast += 1000;
        break;
    case 'w':
        if(nbast>1000) nbast -=1000;
        break;
    case '+':zoom+=0.005;break;
    case '-':                                 
        zoom-=0.005;
        break;
    case '6':
        currentTarget++;
        if(currentTarget >= int(planets.size())){currentTarget = 0;}
        std::cout<<" Vous observez actuellement : "<<planets[currentTarget]<<std::endl;
        break;
    case '4':
        currentTarget--;
        if(currentTarget < 0){currentTarget = planets.size()-1;}
        std::cout<<" Vous observez actuellement : "<<planets[currentTarget]<<std::endl;
        break;
    case '7':
        if(speedFactor > 0.01f && speedFactor <= 1.01f){speedFactor -=0.01f;}
        if(speedFactor > 1.01f){speedFactor -=1.0f;}
        if(speedFactor<=0.01f){speedFactor=0.0f;}
        std::cout<<"Le facteur de vitesse est de : "<<speedFactor<<std::endl;
        break;
    case '8':
        speedFactor = 1.000f;
    case '9':
        if(speedFactor < 10.0f && speedFactor >= 1.01f){speedFactor +=1.0f;}
        if(speedFactor < 1.01f){speedFactor +=0.01f;}
        std::cout<<"Le facteur de vitesse est de : "<<speedFactor<<std::endl;
        break;
    case '*':{
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    }
    case '/':
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    default:
        break;
    }
    idle ();
}

void specialKeys(int key, int x, int y) {
    if(key == GLUT_KEY_LEFT)
		position -= rightVector() * deltaTime * speed;
    else if(key == GLUT_KEY_RIGHT)
		position += rightVector() * deltaTime * speed;
    else if(key == GLUT_KEY_DOWN)
		position -= directionVector() * deltaTime * speed;
    else if(key == GLUT_KEY_UP)
        position += directionVector() * deltaTime * speed;
}

void mouse (int button, int state, int x, int y) {
    if (state == GLUT_UP) {
        mouseMovePressed = false;
        mouseRotatePressed = false;
        mouseZoomPressed = false;
    } else {
        if (button == GLUT_LEFT_BUTTON) {
            //camera.beginRotate (x, y);
            mouseMovePressed = false;
            mouseRotatePressed = true;
            mouseZoomPressed = false;
            lastX = x;
            lastY = y;
        } else if (button == GLUT_RIGHT_BUTTON) {
            lastX = x;
            lastY = y;
            mouseMovePressed = true;
            mouseRotatePressed = false;
            mouseZoomPressed = false;
        } else if (button == GLUT_MIDDLE_BUTTON) {
            if (mouseZoomPressed == false) {
                lastZoom = y;
                mouseMovePressed = false;
                mouseRotatePressed = false;
                mouseZoomPressed = true;
            }
        }
    }
    idle ();
}

void motion (int x, int y) {
    if (mouseRotatePressed == true) {
        computeMatricesFromInputs(x - lastX, y - lastY);
        lastX = x;
        lastY = y;
    }
    else if (mouseMovePressed == true) {
    }
    else if (mouseZoomPressed == true) {
    }
}

void computeMatricesFromInputs(float moveX, float moveY){
    std::cout << moveX << " " << moveY << std::endl;
	// Compute new orientation
	horizontalAngle += mouseSpeed * moveX / 10.f;
	verticalAngle   += mouseSpeed * moveY / 10.f;

	// Up vector
	glm::vec3 up = glm::cross( rightVector(), directionVector() );

	float FoV = initialFoV;

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);

	// Camera matrix
	ViewMatrix       = glm::lookAt(
								camera_position,           // Camera is here
								camera_position + directionVector(), // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );
}


void reshape(int w, int h) {
    // camera.resize (w, h);
}

int main (int argc, char ** argv) {
    if (argc > 2) {
        exit (EXIT_FAILURE);
    }
        // Initialiser le générateur de nombres aléatoires
    std::random_device rd; // Obtenir un nombre aléatoire à partir d'un périphérique
    std::mt19937 gen(rd()); // Utiliser le générateur de Mersenne Twister
    std::uniform_real_distribution<float> dis(0.0, 360.0); // Distribution uniforme entre 0 et 360

    // Remplir langleast avec des flottants aléatoires
    for (int i = 0; i < nbast; i++) {
        langleast.push_back(dis(gen)); // Ajouter un nombre aléatoire au vecteur
    }
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize (SCR_WIDTH, SCR_HEIGHT);
    window = glutCreateWindow ("Solar system");

    init ();
    glutIdleFunc (idle);
    glutDisplayFunc (display);
    glutKeyboardFunc (key);
    glutReshapeFunc (reshape);
    glutMotionFunc (motion);
    glutMouseFunc (mouse);
    glutSpecialFunc(specialKeys);
    key ('?', 0, 0);    
    std::cout<<"hello"<<std::endl;

    computeMatricesFromInputs(0.f, 0.f);

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "vertex_shader.glsl", "fragment_shader.glsl" );

    //Chargement du fichier de maillage
    std::string filename = ("data/sphere.off");
    loadOFF(filename, indexed_vertices, indices, triangles );

    // Load it into a VBO

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    for(int i=0;i<indexed_vertices.size();i++){
        normals.push_back(calculateNormal(indexed_vertices[i]));
        UV.push_back(glm::vec2(0.5f + atan2(indexed_vertices[i].z, indexed_vertices[i].x) / (2 * M_PI),0.5f - asin(indexed_vertices[i].y / (sqrt(pow(indexed_vertices[i].x,2)+pow(indexed_vertices[i].y,2)+pow(indexed_vertices[i].z,2)))) / M_PI));
    }
    // Generate a buffer for the indices as well
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, UV.size() * sizeof(glm::vec2), &UV[0], GL_STATIC_DRAW);

    loadAllTextures();

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    glutMainLoop ();

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);


    return EXIT_SUCCESS;
}
