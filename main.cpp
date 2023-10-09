// Group 9
// SHAON, TANZIR AHMED (1201303219)
// SHAILY, FAHIMA HASSAN (1201303220)
// AHMED SOBHI RAMADAN IBRAHIM ALI BAKAR (1211305402)
#include <GL\glut.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

static int windowWidth = 800;
static int windowHeight = 500;
static float fieldOfView = 60.0;
static float aspectRatio = static_cast<GLdouble>(windowWidth) / windowHeight;
double rotateX = 0.0;
double rotateY = 0.0;
double rotateZ = 0.0;
double posX = 0.0;
double posY = 0.0;
double posZ = 0.0;
double scaleX = 1.0;
double scaleY = 1.0;
double scaleZ = 1.0;

double elephantPosX = -5.0;  // Move the elephant 5 units to the left
double elephantPosY = 0.0;   // Keep the elephant at the same vertical position
double elephantPosZ = 0.0;   // Keep the elephant at the same depth

double tigerPosX = 5.0;      // Move the tiger 5 units to the right
double tigerPosY = 0.0;      // Keep the tiger at the same vertical position
double tigerPosZ = 0.0;      // Keep the tiger at the same depth

static bool mousePanMode = false;
static bool mouseZoomMode = false;
static bool mouseRotationMode = false;
int mouseX, mouseY;
int size = 1000;
static int day = 0;

bool isFogEnabled = false;

GLuint bg_texture, sun_texture, earth_texture, mars_texture, rect_texture, drawing_texture, table_texture;

//--------------------------------------------------------------------------
//Loading texture files, which can also be found at http://www.cppblog.com/doing5552/archive/2009/01/08/71532.aspx
int power_of_two(int n)
{
    if (n <= 0)
        return 0;
    return (n & (n - 1)) == 0;
}
GLuint LoadTexture(const char* filename)
{

    GLint width, height, total_bytes;
    GLubyte* pixels = 0;
    GLuint  texture_ID = 0;


    FILE* pFile = fopen(filename, "rb");
    if (pFile == 0)
        return 0;

    fseek(pFile, 0x0012, SEEK_SET);
    fread(&width, 4, 1, pFile);
    fread(&height, 4, 1, pFile);
    fseek(pFile, 54, SEEK_SET);


    {
        GLint line_bytes = width * 3;
        while (line_bytes % 4 != 0)
            ++line_bytes;
        total_bytes = line_bytes * height;
    }


    pixels = (GLubyte*)malloc(total_bytes);
    if (pixels == 0)
    {
        fclose(pFile);
        return 0;
    }


    if (fread(pixels, total_bytes, 1, pFile) <= 0)
    {
        free(pixels);
        fclose(pFile);
        return 0;
    }


    // The is for the compatibility with old version of opengl
    {
        GLint max;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
        if (!power_of_two(width)
            || !power_of_two(height)
            || width > max
            || height > max)
        {
            const GLint new_width = 512;
            const GLint new_height = 512; // set the picture to 512*512
            GLint new_line_bytes, new_total_bytes;
            GLubyte* new_pixels = 0;


            new_line_bytes = new_width * 3;
            while (new_line_bytes % 4 != 0)
                ++new_line_bytes;
            new_total_bytes = new_line_bytes * new_height;


            new_pixels = (GLubyte*)malloc(new_total_bytes);
            if (new_pixels == 0)
            {
                free(pixels);
                fclose(pFile);
                return 0;
            }


            gluScaleImage(GL_RGB,
                width, height, GL_UNSIGNED_BYTE, pixels,
                new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);


            free(pixels);
            pixels = new_pixels;
            width = new_width;
            height = new_height;
        }
    }


    glGenTextures(1, &texture_ID);
    if (texture_ID == 0)
    {
        free(pixels);
        fclose(pFile);
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
    free(pixels);
    return texture_ID;

}

void init_LoadallTexture()
{
    // D:\\__Ongoing Trimester\\Computer Graphics\\solar-system-redone\\pictures\\bg.bmp
    sun_texture = LoadTexture("D:\\__Ongoing Trimester\\Computer Graphics\\TheFinalProject\\texture_images\\sun.bmp");
    earth_texture = LoadTexture("D:\\__Ongoing Trimester\\Computer Graphics\\TheFinalProject\\texture_images\\earth.bmp");
    mars_texture = LoadTexture("D:\\__Ongoing Trimester\\Computer Graphics\\TheFinalProject\\texture_images\\mars.bmp");
    bg_texture = LoadTexture("D:\\__Ongoing Trimester\\Computer Graphics\\TheFinalProject\\texture_images\\mybg.bmp");
    rect_texture = LoadTexture("D:\\__Ongoing Trimester\\Computer Graphics\\TheFinalProject\\texture_images\\frontWall.bmp");
    drawing_texture = LoadTexture("D:\\__Ongoing Trimester\\Computer Graphics\\TheFinalProject\\texture_images\\draw.bmp");
    table_texture = LoadTexture("D:\\__Ongoing Trimester\\Computer Graphics\\TheFinalProject\\texture_images\\SideWalls.bmp");
   // tflag = LoadTexture("D:\\__Ongoing Trimester\\Computer Graphics\\TheFinalProject\\flag-malaysia.bmp");
}
//--------------------------------------------------------------------------

void get_bg()
{

    glPushMatrix();
    //glRotatef(day/25.0*360, 0.0, 0.0, -1.0);
   // glTranslatef(10.0f, 10.0f, -20.0f);
    // {
    //     GLfloat sun_light_position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    //     GLfloat sun_light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    //     GLfloat sun_light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //     GLfloat sun_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //     glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
    //     glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
    //     glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
    //     glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);

    //     glEnable(GL_LIGHT0);
    //     glEnable(GL_LIGHTING);

    // }
    // {
    //     GLfloat sun_mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    //     GLfloat sun_mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    //     GLfloat sun_mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    //     GLfloat sun_mat_emission[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    //     GLfloat sun_mat_shininess = 0.0f;

    //     glMaterialfv(GL_FRONT, GL_AMBIENT, sun_mat_ambient);
    //     glMaterialfv(GL_FRONT, GL_DIFFUSE, sun_mat_diffuse);
    //     glMaterialfv(GL_FRONT, GL_SPECULAR, sun_mat_specular);
    //     glMaterialfv(GL_FRONT, GL_EMISSION, sun_mat_emission);
    //     glMaterialf(GL_FRONT, GL_SHININESS, sun_mat_shininess);
    // }


    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    glBindTexture(GL_TEXTURE_2D, bg_texture);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere,30, 100, 20);
   // gluSphere(sphere, 0.4, 100, 20);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

float _earth_angle_orbit = 0.0f;       // Earth's position in its orbit around the Sun
float _earth_angle_self = 0.0f;  // Earth's self-rotation angle
float dis_earth_sun = 4.0f;   // Distance from Earth to Sun
float _sun_angle_self = 0.0f;
float _mars_angle_self = 0.0f;
float dis_mars_sun = 6.0f;
float _mars_angle_orbit = 0.0f;

float self_rotation_speed = 1.0f;

void drawSun()
{
    glPushMatrix();

    glRotatef(_sun_angle_self, 0.0, 1.0, 0.0);

    {
        GLfloat sun_light_position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat sun_light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat sun_light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat sun_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);

        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);

    }
    {
        GLfloat sun_mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat sun_mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat sun_mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat sun_mat_emission[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat sun_mat_shininess = 0.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, sun_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, sun_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, sun_mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, sun_mat_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, sun_mat_shininess);
    }


    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    glBindTexture(GL_TEXTURE_2D, sun_texture);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);

    gluSphere(sphere, 0.6, 100, 20);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


void drawEarth()
{

    glPushMatrix();
    glRotatef(_earth_angle_orbit, 0.0, 1.0, 0.0);

    glTranslatef(dis_earth_sun, 0.0, 0.0);

    glRotatef(_earth_angle_self, 0.0, 1.0, 0.0);

    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);

    glBindTexture(GL_TEXTURE_2D, earth_texture);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 0.4, 100, 20);
    glDisable(GL_TEXTURE_2D);
    //glDisable(GL_LIGHT0);
    //glDisable(GL_LIGHTING);
    glPopMatrix();
}

void drawMars()
{

    glPushMatrix();

    glRotatef(_mars_angle_orbit, 0.0, 1.0, 0.0);

    glTranslatef(dis_mars_sun, 0.0, 0.0);
    glRotatef(_mars_angle_self, 0.0, 1.0, 0.0);

    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);

    glBindTexture(GL_TEXTURE_2D, mars_texture);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 0.2, 100, 20);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}


struct Vertex
{
    double x, y, z;
    double nx, ny, nz;
    unsigned char red, green, blue;
};
std::vector<Vertex> vertices_elephant;
std::vector<Vertex> vertices_tiger;

void parsePLY(const std::string &filename, std::vector<Vertex> &vertices)
{
    std::ifstream file(filename);
    std::string line;

    // Skip header lines
    while (std::getline(file, line) && line != "end_header")
        ;

    // Read vertex data

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        Vertex v;
        iss >> v.x >> v.y >> v.z >> v.nx >> v.ny >> v.nz >> v.red >> v.green >> v.blue;
        vertices.push_back(v);
    }
}

void myInit(void)
{
    static GLfloat ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    static GLfloat diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    static GLfloat specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    static GLfloat ref[] = {1.0f, 1.0f, 1.0f, 1.0f};
    static GLfloat position[] = {10.0f, 10.0f, 10.0f, 1.0f};
    short shininess = 128;

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHT0);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glMaterialfv(GL_FRONT, GL_SPECULAR, ref);
    glMateriali(GL_FRONT, GL_SHININESS, shininess);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, aspectRatio, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

    glEnable(GL_DEPTH_TEST);

    // Fog setup
    GLfloat fogColor[] = {0.5f, 0.5f, 0.5f, 1.0f};  // Grey fog
    glFogi(GL_FOG_MODE, GL_LINEAR);                 // The fog Mode
    glFogfv(GL_FOG_COLOR, fogColor);                // Set The fog color
    glFogf(GL_FOG_DENSITY, 0.35f);                  // Set the fog density (for exponential modes)
    glHint(GL_FOG_HINT, GL_DONT_CARE);              // Fog hint value
    glFogf(GL_FOG_START, 5.0f);                     // Fog start distance (for linear mode)
    glFogf(GL_FOG_END, 50.0f);                      // Fog end distance (for linear mode)

    if (isFogEnabled)
    {
        glEnable(GL_FOG);
    }
    else{
        glDisable(GL_FOG);
    }
}
//--------------------------------------------------------------------------
void MyModelAxis(void)
{
    float lengthX = 15.0, lengthY = 15.0, lengthZ = 15.0;
    // glDisable(GL_LIGHTING);

    glEnable(GL_LINE_STIPPLE);
    glLineWidth(1);
    glLineStipple(1, 0xffff);

    glBegin(GL_LINES);
    // x-axis
    glColor3f(1, 0, 0);

    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(lengthX, 0.0, 0.0);
    // x-axis arrow
    glVertex3f(lengthX, 0.0, 0.0);
    glVertex3f(0.9 * lengthX, 0.0, 0.05 * lengthX);
    glVertex3f(lengthX, 0.0, 0.0);
    glVertex3f(0.9 * lengthX, 0.0, -0.05 * lengthX);

    // y-axis
    glColor3f(0, 1, 0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, lengthY, 0.0);
    // y-axis arrow
    glVertex3f(0.0, lengthY, 0.0);
    glVertex3f(0.05 * lengthY, 0.9 * lengthY, 0.0);
    glVertex3f(0.0, lengthY, 0.0);
    glVertex3f(-0.05 * lengthY, 0.9 * lengthY, 0.0);

    // z-axis
    glColor3f(0, 0, 1);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, lengthZ);
    // z-axis arrow
    glVertex3f(0.0, 0.0, lengthZ);
    glVertex3f(0.05 * lengthZ, 0.0, 0.9 * lengthZ);
    glVertex3f(0.0, 0.0, lengthZ);
    glVertex3f(-0.05 * lengthZ, 0.0, 0.9 * lengthZ);
    glEnd();
    // glEnable(GL_LIGHTING);
}
void renderElephant(){
    // Elephant-specific transformations and rendering
            glPushMatrix();
                glTranslatef(elephantPosX, elephantPosY, elephantPosZ); // Set the position for the elephant
                glColor3f(1.0, 1.0, 1.0);
                glScalef(10.0, 10.0, 10.0);
                glBegin(GL_POINTS);
                for (const auto &v : vertices_elephant)
                {
                    glColor3ub(v.red, v.green, v.blue);
                    glVertex3d(v.x, v.y, v.z);
                }
                glEnd();
            glPopMatrix(); // Restore the matrix to the global transformations state
}

void renderTiger(){
    // Tiger-specific transformations and rendering
    glPushMatrix();
        glTranslatef(tigerPosX, tigerPosY, tigerPosZ); // Set the position for the tiger
        glScalef(10.0, 10.0, 10.0); // Adjust scale if needed
        glBegin(GL_POINTS);
        for (const auto &v : vertices_tiger)
        {
            // Base orange color
            float baseR = 1.0f;
            float baseG = 0.5f;
            float baseB = 0.0f;

            // Original color from PLY file
            float origR = v.red / 255.0f;
            float origG = v.green / 255.0f;
            float origB = v.blue / 255.0f;

            // Blend the base orange color with the original color
            float r = (baseR + origR) * 0.5f;
            float g = (baseG + origG) * 0.5f;
            float b = (baseB + origB) * 0.5f;

            glColor3f(r, g, b);
            glVertex3d(v.x, v.y, v.z);
        }
        glEnd();
    glPopMatrix(); // Restore the matrix to the global transformations state
}

void drawSatellite(){

    glPushMatrix();
        glScalef(0.3,0.3,0.3);
        glTranslatef(0.0, 0.0, 10.0);
        // Draw Satellite: One closed cylinder and two rectangular plates
        glColor3f(0.8509f, 0.8509f, 0.8509f); // Set color to red

        // Create the closed cylinder
        // Enable texturing
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, drawing_texture);
        GLUquadricObj *quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL); // Set draw style to fill
        gluCylinder(quadric, 1.0, 1.0, 4.0, 40, 60);
        gluDeleteQuadric(quadric);
        glDisable(GL_TEXTURE_2D);

        // Create the rectangular plates
        glColor3f(0.0157f, 0.2118f, 0.2902f); // Set color to green
        glPushMatrix();
        glTranslatef(0.0, 2.0, 2); // Translate to the top of the cylinder
        glRotatef(90, 0, 0, 1);
        glScalef(2.5, 0.1, 1); // Scale to create the first rectangular plate
        glutSolidCube(2.0);
        glPopMatrix();

        glColor3f(0.0157f, 0.2118f, 0.2902f); // Set color to blue
        glPushMatrix();
        glTranslatef(0.0, -2.0, 2); // Translate to the bottom of the cylinder
        glRotatef(90.0, 0.0, 0.0, 1);
        glScalef(2.5, 0.1, 1); // Scale to create the second rectangular plate
        glutSolidCube(2.0);
        glPopMatrix();
    glPopMatrix();
}

void mySolidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks)
{
    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL); // Set draw style to fill
    gluCylinder(quadric, radius, radius, height, slices, stacks);
    gluDeleteQuadric(quadric);
}

void drawDoor(){
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslatef(0.0, 0.0, 0.0);
        glScalef(2.5, 4.0, 0.5);

        // Enable texturing
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, rect_texture);

        // Manually draw the cube
        glBegin(GL_QUADS);
            // Front face (with texture)
            glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, 0.5);
            glTexCoord2f(1.0, 0.0); glVertex3f( 0.5, -0.5, 0.5);
            glTexCoord2f(1.0, 1.0); glVertex3f( 0.5,  0.5, 0.5);
            glTexCoord2f(0.0, 1.0); glVertex3f(-0.5,  0.5, 0.5);


            glVertex3f(-0.5, -0.5, -0.5);
            glVertex3f( 0.5, -0.5, -0.5);
            glVertex3f( 0.5,  0.5, -0.5);
            glVertex3f(-0.5,  0.5, -0.5);


            glVertex3f(-0.5, -0.5,  0.5);
            glVertex3f(-0.5, -0.5, -0.5);
            glVertex3f(-0.5,  0.5, -0.5);
            glVertex3f(-0.5,  0.5,  0.5);


            glVertex3f( 0.5, -0.5,  0.5);
            glVertex3f( 0.5, -0.5, -0.5);
            glVertex3f( 0.5,  0.5, -0.5);
            glVertex3f( 0.5,  0.5,  0.5);


            glVertex3f(-0.5,  0.5,  0.5);
            glVertex3f( 0.5,  0.5,  0.5);
            glVertex3f( 0.5,  0.5, -0.5);
            glVertex3f(-0.5,  0.5, -0.5);


            glVertex3f(-0.5, -0.5,  0.5);
            glVertex3f( 0.5, -0.5,  0.5);
            glVertex3f( 0.5, -0.5, -0.5);
            glVertex3f(-0.5, -0.5, -0.5);
        glEnd();

        // Disable texturing
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void lockStructure(){
    // intermediate cube
    glPushMatrix();
        glColor3f(0.5, 0.5, 0.5);
        glTranslatef(1.0, 0.0, 0.28);
        glScalef(0.375, 0.25, 0.25);
        glutSolidCube(1.0);
    glPopMatrix();

    // the cylinder
    glPushMatrix();
        glColor3f(0.5, 0.5, 0.4);
        glTranslatef(1.0, 0.0, 0.275);
        mySolidCylinder(0.07, 0.375, 20, 20);
    glPopMatrix();

    // the hand
    glPushMatrix();
        glColor3f(0.5, 0.5, 0.3);
        glTranslatef(0.77, 0.0, 0.685);
        glScalef(0.55, 0.125, 0.05);
        glutSolidCube(1.0);
    glPopMatrix();
}

// Function to draw a rectangle with specified width, height, and depth
void drawRectangularSurface(double width, double height, double depth)
{
    double halfWidth = width / 2.0;
    double halfHeight = height / 2.0;
    double halfDepth = depth / 2.0;

    // Enable texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, table_texture);

    glBegin(GL_QUADS);
    // Bottom face
    glTexCoord2f(0.0, 0.0); glVertex3f(-halfWidth, -0.1, -halfDepth);
    glTexCoord2f(1.0, 0.0); glVertex3f(halfWidth, -0.1, -halfDepth);
    glTexCoord2f(1.0, 1.0); glVertex3f(halfWidth, -0.1, halfDepth);
    glTexCoord2f(0.0, 1.0); glVertex3f(-halfWidth, -0.1, halfDepth);

    // Top face
    glTexCoord2f(0.0, 0.0); glVertex3f(-halfWidth, height - 0.1, -halfDepth);
    glTexCoord2f(1.0, 0.0); glVertex3f(halfWidth, height - 0.1, -halfDepth);
    glTexCoord2f(1.0, 1.0); glVertex3f(halfWidth, height - 0.1, halfDepth);
    glTexCoord2f(0.0, 1.0); glVertex3f(-halfWidth, height - 0.1, halfDepth);

    // Front face
    glTexCoord2f(0.0, 0.0); glVertex3f(-halfWidth, -0.1, -halfDepth);
    glTexCoord2f(1.0, 0.0); glVertex3f(halfWidth, -0.1, -halfDepth);
    glTexCoord2f(1.0, 1.0); glVertex3f(halfWidth, height - 0.1, -halfDepth);
    glTexCoord2f(0.0, 1.0); glVertex3f(-halfWidth, height - 0.1, -halfDepth);

    // Back face
    glTexCoord2f(0.0, 0.0); glVertex3f(-halfWidth, -0.1, halfDepth);
    glTexCoord2f(1.0, 0.0); glVertex3f(halfWidth, -0.1, halfDepth);
    glTexCoord2f(1.0, 1.0); glVertex3f(halfWidth, height - 0.1, halfDepth);
    glTexCoord2f(0.0, 1.0); glVertex3f(-halfWidth, height - 0.1, halfDepth);

    // Left face
    glVertex3f(-halfWidth, -0.1, -halfDepth);
    glVertex3f(-halfWidth, -0.1, halfDepth);
    glVertex3f(-halfWidth, height - 0.1, halfDepth);
    glVertex3f(-halfWidth, height - 0.1, -halfDepth);

    // Right face
    glVertex3f(halfWidth, -0.1, -halfDepth);
    glVertex3f(halfWidth, -0.1, halfDepth);
    glVertex3f(halfWidth, height - 0.1, halfDepth);
    glVertex3f(halfWidth, height - 0.1, -halfDepth);

    glEnd();
    // Disable texturing
    glDisable(GL_TEXTURE_2D);
}

void tableDrawing(){

// Draw the table top (a rectangle)
    glPushMatrix(); // Save the current matrix for the table top
    glTranslatef(0.0, 0.55, 0.0); // Translate the table top above the legs
    drawRectangularSurface(4.0, 0.1, 1.25);
    glPopMatrix(); // Restore the matrix for the table top


    glPushMatrix(); // Save the current matrix for the first leg
    glColor3f(0.9373f, 0.9373f, 0.9373f); // Set the color for the legs
    glTranslatef(1.65, 0.5, 0.4); // Position the first leg
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    mySolidCylinder(0.1, 1.25, 20, 20); // Draw the first leg
    glPopMatrix(); // Restore the matrix for the first leg

    glPushMatrix(); // Save the current matrix for the second leg
    glColor3f(0.9373f, 0.9373f, 0.9373f); // Set the color for the legs
    glTranslatef(1.65, 0.5, -0.4); // Position the second leg
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    mySolidCylinder(0.1, 1.25, 20, 20); // Draw the second leg
    glPopMatrix(); // Restore the matrix for the second leg

    glPushMatrix(); // Save the current matrix for the third leg
    glColor3f(0.9373f, 0.9373f, 0.9373f); // Set the color for the legs
    glTranslatef(-1.65, 0.5, 0.4); // Position the third leg
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    mySolidCylinder(0.1, 1.25, 20, 20); // Draw the third leg
    glPopMatrix(); // Restore the matrix for the third leg

    glPushMatrix(); // Save the current matrix for the fourth leg
    glColor3f(0.9373f, 0.9373f, 0.9373f); // Set the color for the legs
    glTranslatef(-1.65, 0.5, -0.4); // Position the fourth leg
    glRotatef(90.0, 1.0, 0.0, 0.0);
    mySolidCylinder(0.1, 1.25, 20, 20); // Draw the fourth leg
    glPopMatrix(); // Restore the matrix for the fourth leg

}

// Chair drawing
void chairDrawing(){
    // drawing the chair back
    glPushMatrix();
    glTranslatef(0.0, 0.15, 2.1);
    glColor3f(0.5, 0.5, 0.5);
    glScalef(1.0, 1.5, 0.05);

    drawRectangularSurface(1.0, 1.0, 1.0);
    glPopMatrix();

    // drawing the chair base
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);
    glTranslatef(0.0, 0.0, 1.5);
    glScalef(1.0, 0.05, 1.25);
    drawRectangularSurface(1.0, 1.0, 1.0);
    glPopMatrix();

    // drawing legs
    // first leg
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(0.35, 0.0, 2.0); // x not z
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glScalef(0.4, 0.4, 0.4);
    mySolidCylinder(0.1, 2.5, 20, 20);
    glPopMatrix();


    // second leg
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(0.35, 0.0, 1.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glScalef(0.4, 0.4, 0.4);
    mySolidCylinder(0.1, 2.5, 20, 20);
    glPopMatrix();


    // third leg
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(-0.35, 0.0, 2.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glScalef(0.4, 0.4, 0.4);
    mySolidCylinder(0.1, 2.5, 20, 20);
    glPopMatrix();


    // fourth leg
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(-0.35, 0.0, 1.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glScalef(0.4, 0.4, 0.4);
    mySolidCylinder(0.1, 2.5, 20, 20);
    glPopMatrix();
}

void myDisplayFunc(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glPushMatrix(); // Save the current matrix

        // Apply transformations
        glTranslatef(posX, posY, posZ);
        glRotatef(rotateX, 1.0, 0.0, 0.0);
        glRotatef(rotateY, 0.0, 1.0, 0.0);
        glRotatef(rotateZ, 0.0, 0.0, 1.0);
        glScalef(scaleX, scaleY, scaleZ);

        get_bg();
        //renderElephant();

        glPushMatrix();
            glTranslatef(5.0, 5.0, 0.0);
            drawSatellite();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-10.0, 0.0, 0.0);
            drawDoor();
            lockStructure();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(10.0, 0.0, 0.0);
            tableDrawing();
            chairDrawing();
        glPopMatrix();
       // renderTiger();

        // Render the axes
        //MyModelAxis();
        glPushMatrix();
            glTranslatef(0.0, 0.0, 2.0);
            drawSun();
        glPopMatrix();

        drawEarth();

        drawMars();

    glPopMatrix(); // Restore the matrix

    glutSwapBuffers();
}



//--------------------------------------------------------------------------
void myReshapeFunc(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}
//--------------------------------------------------------------------------
void myKeyboardFunc(unsigned char key, int x, int y)
{
    switch (key)
    {
    
    case 'I':
    case 'i':
        self_rotation_speed += 0.3;
        break;
    case 'D':
    case 'd':
        self_rotation_speed -= 0.3;
        if (self_rotation_speed < 0.1f)  // Avoid negative speeds
            self_rotation_speed = 0.1f;
        break;
    case 'F':
    case 'f':
        isFogEnabled = !isFogEnabled;
        if (isFogEnabled)
        {
            glEnable(GL_FOG);
        }
        else
        {
            glDisable(GL_FOG);
        }
        break;
    case 27:
        exit(1);
        break;
    }
    glutPostRedisplay();
}
//--------------------------------------------------------------------------
void mySpecialFunc(int key, int x, int y)
{
    switch (key)
    {

    case GLUT_KEY_F1:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case GLUT_KEY_F2:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    }
    glutPostRedisplay();
}
//--------------------------------------------------------------------------
void myMouseFunc(int button, int state, int x, int y)
{
    y = windowHeight - y;
    switch (button)
    {
    case GLUT_MIDDLE_BUTTON:
        if (state == GLUT_DOWN && !mousePanMode)
        {
            mouseX = x;
            mouseY = y;
            mousePanMode = true;
        }
        if (state == GLUT_UP && mousePanMode)
        {
            mousePanMode = false;
        }
        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN && !mouseZoomMode)
        {
            mouseY = y;
            mouseZoomMode = true;
        }
        if (state == GLUT_UP && mouseZoomMode)
        {
            mouseZoomMode = false;
        }
        break;
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN && !mouseRotationMode)
        {
            mouseX = x;
            mouseY = y;
            mouseRotationMode = true;
        }
        if (state == GLUT_UP && mouseRotationMode)
        {
            mouseRotationMode = false;
        }
        break;
    }
}
//--------------------------------------------------------------------------

void myMotionFunc(int x, int y)
{
    y = windowHeight - y;
    if (mousePanMode)
    {
        posX += (x - mouseX) * 0.01;
        posY += (y - mouseY) * 0.01;
        mouseX = x;
        mouseY = y;
        glutPostRedisplay();
    }
    if (mouseZoomMode)
    {
        GLdouble inc = (mouseY - y) * 0.01;
        scaleX += inc;
        scaleY += inc;
        scaleZ += inc;
        if (scaleX < 0.1)
            scaleX = 0.1;
        if (scaleY < 0.1)
            scaleY = 0.1;
        if (scaleZ < 0.1)
            scaleZ = 0.1;
        mouseY = y;
        glutPostRedisplay();
    }
    if (mouseRotationMode)
    {
        rotateX += (mouseY - y) * 0.5;
        rotateY += (x - mouseX) * 0.5;
        mouseX = x;
        mouseY = y;
        glutPostRedisplay();
    }
}
//--------------------------------------------------------------------------
// Update the Earth's angles
void update(int value) {
    _earth_angle_orbit += 0.5f;  // Adjust this to change Earth's orbital speed
    if (_earth_angle_orbit > 360) _earth_angle_orbit -= 360;

    _earth_angle_self += self_rotation_speed;  // Adjust this to change Earth's self-rotation speed
    if (_earth_angle_self > 360) _earth_angle_self -= 360;

    _sun_angle_self += self_rotation_speed;
    if (_sun_angle_self > 360) _sun_angle_self -= 360;

    _mars_angle_self += self_rotation_speed;
    if (_mars_angle_self > 360) _mars_angle_self -= 360;

    _mars_angle_orbit += 0.3f;
    if (_mars_angle_orbit > 360) _mars_angle_orbit -= 360;

    glutPostRedisplay(); // Request a redraw of the window
    glutTimerFunc(25, update, 0);  // Reset the timer 25 miliseconds
}

void displayUserGuide() {
    std::cout << "=================== User Guide ===================" << std::endl;
    std::cout << std::endl;

    // Keyboard instructions
    std::cout << "Keyboard:" << std::endl;
    std::cout << "Press 'i' to Increase Self Rotation Speed of the Planets." << std::endl;
    std::cout << "Press 'j' to Decrease Self Rotation Speed of the Planets." << std::endl;
    std::cout << "Press 'f' to toggle fog." << std::endl;
    std::cout << std::endl;

    // Mouse instructions
    std::cout << "Mouse:" << std::endl;
    std::cout << "Left Button: Rotate the scene." << std::endl; // Example, replace with actual functionality
    std::cout << "Middle Button: Pan the scene." << std::endl; // Example, replace with actual functionality
    std::cout << "Right Button: Zoom in/out." << std::endl;    // Example, replace with actual functionality
    std::cout << std::endl;

    // Exiting the program
    std::cout << "Press ESC to escape." << std::endl;
    std::cout << std::endl;
    std::cout << "=================================================" << std::endl;
}

int main(int argc, char **argv)
{
    displayUserGuide();

    // Load the elephant
    // std::string file_elephant= "D:\\__Ongoing Trimester\\Computer Graphics\\Projects\\Elephant\\elephant-point-cloud-text.ply";
    // parsePLY(file_elephant, vertices_elephant);

    // Load the tiger
    // std::string file_tiger= "D:\\__Ongoing Trimester\\Computer Graphics\\Projects\\Tiger\\tiger-point-cloud-text.ply";
    // parsePLY(file_tiger, vertices_tiger);

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Computer Graphics Fundamentals | G9 | Dr Ng Kok Why");

    init_LoadallTexture();
    myInit();

    glutDisplayFunc(myDisplayFunc);
    glutReshapeFunc(myReshapeFunc);
    // glutTimerFunc(50, timerProc, 1);
    glutKeyboardFunc(myKeyboardFunc);
    glutSpecialFunc(mySpecialFunc);
    glutMotionFunc(myMotionFunc);
    glutMouseFunc(myMouseFunc);

    glutTimerFunc(25, update, 0); //Timer for animation

    glutMainLoop(); // Display everything and wait
    return 0;
}
