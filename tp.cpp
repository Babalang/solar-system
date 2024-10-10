// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <random>

// Include GLEW
#include <GL/glew.h>

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

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 camera_position   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);

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
GLuint LightID;


std::vector<unsigned short> indices; //Triangles concaténés dans une liste
std::vector<std::vector<unsigned short> > triangles;
std::vector<glm::vec3> indexed_vertices;
std::vector<glm::vec3> normals;


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

//Fonction pour afficher les différents astres :
mat4 affichePlanete(mat4 Model_matrix, glm::vec3 Color, float revolution, float SunRotPerMinutes, float distance, float inclinaison, float NbRotation, float scale, GLuint loc, GLuint locColor){
    glUniform3fv(glGetUniformLocation(programID, "objectColor"), 1, &Color[0]);
    Model_matrix = glm::rotate(Model_matrix,glm::radians(revolution * SunRotPerMinutes),glm::vec3(0.0f,1.0f,0.0f));
    Model_matrix = glm::translate(Model_matrix,glm::vec3(distance,0.0f,0.0f));
    Model_matrix = glm::rotate(Model_matrix, glm::radians(inclinaison),glm::vec3(0.0f,0.0f,1.0f));
    mat4 Model_matrix1 = Model_matrix;
    Model_matrix = glm::rotate(Model_matrix, glm::radians(NbRotation * SunRotPerMinutes),glm::vec3(0.0f,1.0f,0.0f));
    Model_matrix = glm::scale(Model_matrix,glm::vec3(scale,scale,scale));
    glUniformMatrix4fv(loc,1,GL_FALSE, &Model_matrix[0][0]);
    glUniform3f(locColor,Color[0]/255.0f, Color[1]/255.0f, Color[2]/255.0f);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_SHORT,(void*)0);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    return Model_matrix1;
}

glm::vec3 calculateNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    glm::vec3 edge1 = v2 - v1;
    glm::vec3 edge2 = v3 - v1;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    return normal;
}



// ------------------------------------
// rendering.
// ------------------------------------
bool exo1 = true;
bool exo2 = false;
bool exo3 = false;
int currentTarget =0;
float SunRotPerMinutes = 1.0f;
std::string currentFilename = "data/chair.off"; // Fichier par défaut
void draw () {
    glUseProgram(programID);
    if(exo1){
        float zoom1 = 1.;
        float tx1 = 0.75;
        float ty1 = -1.;
        mat4 Model_matrix(1.*zoom1,0.,0.,0.,
                      0.,1.*zoom1,0.,0.,
                      0.,0.,1.,0.,
                      0.-tx1,0.+ty1,0.,1.);
        ViewMatrix = glm::lookAt(camera_position,camera_target,camera_up);
        ProjectionMatrix = glm::perspective(45.0f,4.0f/3.0f,0.1f,100.0f);
        GLuint loc = glGetUniformLocation(programID, "Model");
        glUniformMatrix4fv(loc,1,GL_FALSE, &Model_matrix[0][0]);
        GLuint locView = glGetUniformLocation(programID, "View");
        glUniformMatrix4fv(locView,1,GL_FALSE, &ViewMatrix[0][0]);
        GLuint locProj = glGetUniformLocation(programID, "Projection");
        glUniformMatrix4fv(locProj,1,GL_FALSE, &ProjectionMatrix[0][0]);
        GLuint locColor = glGetUniformLocation(programID, "Color_Vec");
        glUniform3f(locColor,255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
        
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_SHORT,(void*)0);
        glDisableVertexAttribArray(0);

        // Afficher une seconde chaise
        Model_matrix = mat4(-1.*zoom1,0.,0.,0.,0.,1.*zoom1,0.,0.,0.,0.,1.,0.,0.+tx1,0.+ty1,0.,1.);
        glUniformMatrix4fv(loc,1,GL_FALSE, &Model_matrix[0][0]);
        glUniform3f(locColor,255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_SHORT,(void*)0);
        glDisableVertexAttribArray(0);
        // Afficher une troisieme chaise!
        Model_matrix = mat4(1.*zoom,0.,0.,0.,0.,1.*zoom,0.,0.,0.,0.,1.,0.,0.+tx,0.5+ty,0.,1.);
        Model_matrix = glm::rotate(Model_matrix,angle, glm::vec3(0.0f,0.0f,1.0f));
        Model_matrix = glm::translate(Model_matrix,glm::vec3(0.,-(1.0*zoom)/2.,0.));
        glUniformMatrix4fv(loc,1,GL_FALSE, &Model_matrix[0][0]);
        glUniform3f(locColor,255.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_SHORT,(void*)0);
        glDisableVertexAttribArray(0);
    }

    if(exo2){
        mat4 Model_matrix(1.*zoom,0.,0.,0.,
                      0.,1.*zoom,0.,0.,
                      0.,0.,1.*zoom,0.,
                      0.+tx,0.+ty,0.,1.);
        Model_matrix = glm::rotate(Model_matrix, -float(M_PI/4),glm::vec3(1.f, 0.0f, 0.f));
        Model_matrix = glm::rotate(Model_matrix, -float(M_PI/4),glm::vec3(0.f, 0.0f, 1.f));
        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up
        ViewMatrix = glm::lookAt(camera_position,camera_target,camera_up);
        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        ProjectionMatrix = glm::perspective(45.0f,4.0f/3.0f,0.1f,100.0f);
            // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms
        GLuint loc = glGetUniformLocation(programID, "Model");
        glUniformMatrix4fv(loc,1,GL_FALSE, &Model_matrix[0][0]);
        GLuint locView = glGetUniformLocation(programID, "View");
        glUniformMatrix4fv(locView,1,GL_FALSE, &ViewMatrix[0][0]);
        GLuint locProj = glGetUniformLocation(programID, "Projection");
        glUniformMatrix4fv(locProj,1,GL_FALSE, &ProjectionMatrix[0][0]);
        GLuint locColor = glGetUniformLocation(programID, "Color_Vec");
        glUniform3f(locColor,0.0f/255.0f, 255.0f/255.0f, 0.0f/255.0f);
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );
        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        // Draw the triangles !
        glDrawElements(
                    GL_TRIANGLES,      // mode
                    indices.size(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
                    );
        glDisableVertexAttribArray(0);
    }
    if(exo3){
                // Position de la lumière (ex: le Soleil)
        glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
        glm::vec3 lightColor(10.0f, 10.0f, 8.0f); // Couleur jaune
        glm::vec3 objectColor(1.0f, 1.0f, 0.8f); // Une couleur visible
        glUniform3fv(glGetUniformLocation(programID, "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(programID, "lightColor"), 1, &lightColor[0]);
        glUniform3fv(glGetUniformLocation(programID, "objectColor"), 1, &objectColor[0]);

        std::vector<mat4> lplanet;
        ProjectionMatrix = glm::perspective(45.0f,4.0f/3.0f,0.1f,10000.0f);
        GLuint locProj = glGetUniformLocation(programID, "Projection");
        glUniformMatrix4fv(locProj,1,GL_FALSE, &ProjectionMatrix[0][0]);
        mat4 Model_matrix_Sol(1.,0.,0.,0.,
                      0.,1.,0.,0.,
                      0.,0.,1.,0.,
                      0.,0.,0.,1.);
        GLuint loc = glGetUniformLocation(programID, "Model");
        GLuint locColor = glGetUniformLocation(programID, "Color_Vec");
        //Affichage du Soleil
        Model_matrix_Sol = affichePlanete(Model_matrix_Sol,glm::vec3(255.0f,165.0f,0.0f), 0.0f, SunRotPerMinutes, 0.0f, 0.0f, angleSol, 20.0f,loc,locColor);
        lplanet.push_back(Model_matrix_Sol);

        //Système solaire
            //Affichage de la Terre
            mat4 Model_matrix_Ter = Model_matrix_Sol;
            Model_matrix_Ter = affichePlanete(Model_matrix_Ter, glm::vec3(0.0f,102.0f,204.0f), angleTer, SunRotPerMinutes, 100.0f, 23.44f, angleRotTer, 1.0f,loc,locColor);
            lplanet.push_back(Model_matrix_Ter);
            //Système planétaire
                //affichage de la Lune
                mat4 Model_matrix_Moon = Model_matrix_Ter;
                Model_matrix_Moon = glm::rotate(Model_matrix_Moon,glm::radians(-18.3f),glm::vec3(0.0f,0.0f,1.0f));
                Model_matrix_Moon = affichePlanete(Model_matrix_Moon, glm::vec3(192.0f), angleMoon, SunRotPerMinutes, 3.0f, 6.68f, angleMoon, 0.27f, loc,locColor);
                lplanet.push_back(Model_matrix_Moon);
            //affichage de Mercure
            mat4 Model_matrix_Mer = Model_matrix_Sol;
            Model_matrix_Mer = affichePlanete(Model_matrix_Mer, glm::vec3(169.0f), angleMer, SunRotPerMinutes, 39.0f, 0.03f, angleRotMer, 0.38f, loc,locColor);
            lplanet.push_back(Model_matrix_Mer);
            //Affichage de Venus
            mat4 Model_matrix_Ven = Model_matrix_Sol;
            Model_matrix_Ven = affichePlanete(Model_matrix_Ven, glm::vec3(255.0f,239.0f,0.0f), angleVen, SunRotPerMinutes, 72.0f, 177.4f, angleRotVen, 0.95f,loc,locColor);
            lplanet.push_back(Model_matrix_Ven);
            //Affichage de Mars
            mat4 Model_matrix_Mars = Model_matrix_Sol;
            Model_matrix_Mars = affichePlanete(Model_matrix_Mars, glm::vec3(255.0f,99.0f,71.0f), angleMars, SunRotPerMinutes, 152.0f, 25.2f, angleRotMars, 0.53f,loc,locColor);
            lplanet.push_back(Model_matrix_Mars);
            //Affichage de Jupiter
            mat4 Model_matrix_Jup = Model_matrix_Sol;
            Model_matrix_Jup = affichePlanete(Model_matrix_Jup, glm::vec3(178.0f, 132.0f, 84.0f), angleJup, SunRotPerMinutes, 520.0f, 3.13f, angleRotJup, 11.2f,loc,locColor);
            lplanet.push_back(Model_matrix_Jup);
            //Affichage de Saturne
            mat4 Model_matrix_Sat = Model_matrix_Sol;
            Model_matrix_Sat = affichePlanete(Model_matrix_Sat, glm::vec3(255.0f, 221.0f, 185.0f), angleSat, SunRotPerMinutes, 958.0f, 27.0f, angleRotSat, 9.45f,loc,locColor);
            lplanet.push_back(Model_matrix_Sat);
            //Affichage de Uranus
            mat4 Model_matrix_Uranus = Model_matrix_Sol;
            Model_matrix_Uranus = affichePlanete(Model_matrix_Uranus, glm::vec3(173.0f, 216.0f, 230.0f), angleUranus, SunRotPerMinutes, 1920.0f, 97.8f, angleRotUranus, 4.0f,loc,locColor);
            lplanet.push_back(Model_matrix_Uranus);
            //Affichage de Neptune
            mat4 Model_matrix_Nept = Model_matrix_Sol;
            Model_matrix_Nept = affichePlanete(Model_matrix_Nept, glm::vec3(130.0f, 144.0f, 25.0f), angleNept, SunRotPerMinutes, 3005.0f, 28.0f, angleRotNept, 3.88f,loc,locColor);
            lplanet.push_back(Model_matrix_Nept);     
            //Affichage astéroides
            float a =0.;
            for(int i =0;i<nbast/2;i++){
                if (i % 60 == 0 && i != 0) { // Assurez-vous de ne pas incrémenter a à 0
                    a++;
                }
                mat4 Model_matrix_Ast = Model_matrix_Sol;
                Model_matrix_Ast = affichePlanete(Model_matrix_Ast,glm::vec3(200.0f,200.0f,200.0f),langleast[i],SunRotPerMinutes,200.0f +(5.0f*a*(3600.0f/(float(nbast)/2))),0.0f,0.0f,0.5f,loc,locColor);    
            }
            for(int i =0;i<nbast;i++){
                if (i % 60 == 0 && i != 0) { // Assurez-vous de ne pas incrémenter a à 0
                    a++;
                }
                mat4 Model_matrix_Ast = Model_matrix_Sol;
                Model_matrix_Ast = affichePlanete(Model_matrix_Ast,glm::vec3(250.0f,250.0f,250.0f),langleast[i],SunRotPerMinutes,3050.0f +(10.0f*a*(3600.0f/float(nbast))),0.0f,0.0f,0.6f,loc,locColor);    
            }
        float zom = zoom * 500;
        if(currentTarget < float(lplanet.size())){
            camera_position = (glm::vec3(lplanet[currentTarget][3]) + glm::vec3(15.0f+zom,15.0f+zom,0.0f));   
            camera_target = glm::vec3(lplanet[currentTarget][3]);    
        }
            
        ViewMatrix = glm::lookAt(camera_position,camera_target,camera_up);
        GLuint locView = glGetUniformLocation(programID, "View");
        glUniformMatrix4fv(locView,1,GL_FALSE, &ViewMatrix[0][0]);
    }
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
    if(exo3){
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
        for(int i =0;i<nbast;i++){
            langleast[i] += deltaTime * (0.1f)*speedFactor;
            if(langleast[i]>360.0f) langleast[i]-=360.0f;
        }
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
    case 's':camera_position -= cameraSpeed * camera_target;break;
    case 'w':camera_position += cameraSpeed * camera_target;break;
    case 'c':camera_position -= cameraSpeed * (vec3(1.0,0.,0.));break;
    case 'v':camera_position += cameraSpeed * (vec3(1.0,0.,0.));break;

    case '+':zoom+=0.005;break;
    case '-':                                 
        if(zoom>0.005 || exo3){zoom-=0.005;}
        break;
    
    case 'd':tx+=0.005;break;
    case 'q':tx-=0.005;break;
    case 'z':ty+=0.005;break;
    case 'x':ty-=0.005;break;
    case 'a':angle+=0.1;break;
    case 'e':angle-=0.1;break;
    
    case '1':
        glClearColor (0.2f, 0.2f, 0.3f, 1.0f);
        zoom = 1.0f;
        camera_position   = glm::vec3(0.0f, 0.0f,  3.0f);
        camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
        camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);
        exo1 = true;
        exo2 = false;
        exo3 = false;
        RechargeOFF("data/chair.off");
        break;

    case '2':
        glClearColor (0.2f, 0.2f, 0.3f, 1.0f);
        zoom = 1.0f;
        camera_position   = glm::vec3(0.0f, 0.0f,  3.0f);
        camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
        camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);
        exo1 = false;
        exo2 = true;
        exo3 = false;
        RechargeOFF("data/suzanne.off");
        break;
    case '3':
        glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
        zoom = 0.0f;
        exo1 = false;
        exo2 = false;
        exo3 = true;
        RechargeOFF("data/sphere.off");
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
        speedFactor = 1.0f;
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
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize (SCR_WIDTH, SCR_HEIGHT);
    window = glutCreateWindow ("TP HAI719I");

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

    // Initialiser le générateur de nombres aléatoires
    std::random_device rd; // Obtenir un nombre aléatoire à partir d'un périphérique
    std::mt19937 gen(rd()); // Utiliser le générateur de Mersenne Twister
    std::uniform_real_distribution<float> dis(0.0, 360.0); // Distribution uniforme entre 0 et 360

    // Remplir langleast avec des flottants aléatoires
    for (int i = 0; i < nbast; i++) {
        langleast.push_back(dis(gen)); // Ajouter un nombre aléatoire au vecteur
    }
    computeMatricesFromInputs(0.f, 0.f);

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "vertex_shader.glsl", "fragment_shader.glsl" );

    //Chargement du fichier de maillage
    std::string filename = ("data/chair.off");
    loadOFF(filename, indexed_vertices, indices, triangles );

    // Load it into a VBO

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);


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
