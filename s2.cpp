#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <GL\glew.h>
#include <GL\freeglut.h>   // handle the window-managing operations
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
using namespace std;
#define PI 3.1415926535898

GLfloat BXMAX, BXMIN, BYMAX, BYMIN;
GLfloat TBXMAX, TBXMIN;
GLint circle_points = 100;
float rd = 0.2;  //change the size of ball
GLfloat x = 0.0, y = 0.0, xt = 0.0, yt = -0.8;
GLdouble l, r, b, t;
GLfloat m = 0.0001f;
GLfloat n = 0.0003f;
int p, q;
int flag = 0, level = 1;
int cnt = 0;
const char* d;
const char* de;

GLuint texture[3];

GLuint LoadTexture(const char* filename)
{
    GLuint texture;

    FILE* f = fopen(filename, "rb");

    // A bmp image consist of bmp header and image data
    // First 54 bytes are header information , the rest is image data
    // In the header information , image width is stored in 18th index and image height is stored in 22th index
    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, f);

    // extract image height and width from header
    int width = *(int*)&header[18];
    int height = *(int*)&header[22];

    // allocate 3 bytes per pixel ( R,G,B )
    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size];

    // read the rest of the image data at once
    fread(data, sizeof(unsigned char), size, f);
    fclose(f);

    // reorder the image colors to RGB not BGR
    int i;
    for (i = 0; i < size; i += 3)
    {
        // flip the order of every 3 bytes
        unsigned char tmp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = tmp;
    }


    glGenTextures(1, &texture);            //generate the texture with the loaded data
    glBindTexture(GL_TEXTURE_2D, texture); //bind the texture to it's array

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); //set texture environment parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);   // Image data to texture

    glBindTexture(GL_TEXTURE_2D, 0);    // Unbind texture

    free(data); //free the texture array

    if (glGetError() != GL_NO_ERROR)
        printf("GLError in genTexture()\n");

    return texture; //return whether it was successfull
}


// fuction to intialz basic window
void initGL() {
    glClearColor(0.5, 1.0, 0.8, 1.0); //color of background
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glEnable(GL_LINE_SMOOTH);

    // Load images to texture
    texture[0] = LoadTexture("Data\\Image\\Ball.bmp");  // Loading image of ball to  texture
    texture[1] = LoadTexture("Data\\Image\\Bg.bmp");    // Loading image of background to  texture
    texture[2] = LoadTexture("Data\\Image\\Board.bmp");   // Loading image of board to  texture
}

//fuction to print string of characters on the window screen
void print(int x, int y, int z, const char* string) {
    glRasterPos2f(x, y);
    int i;
    int len = (int)strlen(string);

    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
        //in built func to print the text bit vz
    }
}

/*idle function to show the current picture on the screen while next
one being drawn in processor */
void idle() {
    glutPostRedisplay();
}

// display function
void display() {
    // Draw Background
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0); //the color of board
    glEnable(GL_TEXTURE_2D);    // Enable texture for drawing
    glBindTexture(GL_TEXTURE_2D, texture[1]);   // Binding texture
    // Drawing a quad with texture mapping
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0);    // Top Left
    glVertex2f(l, t);
    glTexCoord2f(1.0, 1.0);    // Top Right
    glVertex2f(r, t);
    glTexCoord2f(1.0, 0.0);    // Bottom Right
    glVertex2f(r, b);
    glTexCoord2f(0.0, 0.0);    // Bottom Left 
    glVertex2f(l, b);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);    // Unbinding Texture
    glPopMatrix();

    if (flag == 0)
    {

        /* in order to draw multiple pics on the screen we use Push matrix to
        push each pic on the buffer */

        // Draw Board
        glPushMatrix();
        glTranslatef(xt, yt, 0.0);
        glColor3f(0.0, 0.0, 0.0); //the color of board
        glEnable(GL_TEXTURE_2D);    // Enable texture for drawing
        glBindTexture(GL_TEXTURE_2D, texture[2]);   // Binding texture
        // Drawing a quad with texture mapping
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);      // Top Left
        glVertex2f(-0.3, 0.1);
        glTexCoord2f(1.0, 1.0);     // Top Right
        glVertex2f(0.3, 0.1);
        glTexCoord2f(1.0, 0.0);     // Bottom Right
        glVertex2f(0.3, -0.1);
        glTexCoord2f(0.0, 0.0);     // Bottom Left 
        glVertex2f(-0.3, -0.1);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);    // Unbind texture
        // we pop that pic to screen
        glPopMatrix();

        // we push next pic
        glPushMatrix();
        glTranslatef(x, y, 0.00);
        // Draw Ball
        int i;
        glColor3f(0.5, 0.3, 0.2); //color of ball
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBegin(GL_POLYGON);
        // this is to draw a circle on the screen
        for (i = 0; i < circle_points; i++)
        {
            float angle = 2 * PI * i / circle_points;
            glTexCoord2f(((cos(angle)) * 4.9 * rd + 1.0) / 2.0, ((sin(angle)) * 4.9 * rd + 1.0) / 2.0); // Mapping texture to circle
            glVertex2f((cos(angle)) * rd, (sin(angle)) * rd);
        }
        glEnd();
        //pop that circle on the screen
        glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();

        // push score board to buffer
        glPushMatrix();
        glColor3f(0.0, 0.0, 1.0);
        glTranslatef(0.6, 0.92, 0.00);

        //pop that to screen
        glPopMatrix();


        // display the score
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);
        glTranslatef(0.98, 0.92, 0.00);
        switch (cnt) {
        case 0: d = "00"; break;
        case 1: d = "01"; break;
        case 2: d = "02"; break;
        case 3: d = "03"; break;
        case 4: d = "04"; break;
        case 5: d = "05"; break;
        case 6: d = "06"; break;
        case 7: d = "07"; break;
        case 8: d = "08"; break;
        case 9: d = "09"; break;
        case 10: d = "10"; break;
        case 11: d = "11"; break;
        case 12: d = "12"; break;
        case 13: d = "13"; break;
        case 14: d = "14"; break;
        default: d = "00";
        }
        print(0, 0, 0, d);
        glPopMatrix();

        // level display
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);
        glTranslatef(-0.6, 0.92, 0.00);

        glPopMatrix();

        // value of level
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);
        glTranslatef(-0.3, 0.92, 0.00);
        switch (level) {

        case 1: de = "01"; break;
        case 2: de = "02"; break;
        case 3: de = "03"; break;
        case 4: de = "04"; break;
        case 5: de = "05"; break;
        case 6: de = "06"; break;
        case 7: de = "07"; break;
        case 8: de = "08"; break;
        case 9: de = "09"; break;
        case 10: de = "10"; break;
        case 11: de = "11"; break;
        case 12: de = "12"; break;
        default: de = "00";

        }
        print(0, 0, 0, de);
        glPopMatrix();
        // next instruction swap the content of display buffer with the next pic to be displayed
        glutSwapBuffers();


        if (x > BXMAX) { x = BXMAX; m = -m; }
        else if (x < BXMIN) { x = BXMIN; m = -m; }
        if (y > BYMAX) { y = BYMAX;  n = -n; }
        else if (y < BYMIN) { y = BYMIN; n = -n; }

        float f;
        float a1, b1{}, a2, b2;
        a1 = ((xt)-(x));
        b1 == ((yt)-(y));
        a2 = pow(a1, 2);
        b2 = pow(b1, 2);
        f = sqrt(a2 + b2);

        if (f <= 0.424264068 && y <= -0.5 && y >= -0.7) {
            n = -1.002 * n; //bounce back the ball if it touches the bar
            cnt++;    // increase the score by 1
            if (cnt == 15) {
                cnt = 0;
                level++; // next level
                m = 1.5 * m; //incr the speed of ball in x-axis
                n = 1.5 * n; //incr the speed of ball in y-axis
            }
        }
        if ((y < -0.55 && f < 0.5831 && f > 0.5)) {
            x = x; y = y; m = 0; n = 0; flag = 1; // game over

        }
        else {
            if (y <= -0.6)
            {
                x = x; y = y; m = 0; n = 0; flag = 1; // game over
            }
            else {
                //move the base rect to left
                p = xt - 0.3;
                //move the base rect to right
                q = xt + 0.3;
                // increase the speed of ball in x axis
                x = x + m;
                // increase the speed of ball in y axis
                y = y + n;
            }
        }
    }
    else
    {
        // Display Game Over
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);
        glTranslatef(-0.15, 0.0, 0.00);
        print(0, 0, 0, "Game Over");
        glPopMatrix();
        glutSwapBuffers();
    }
}


void reshape(int width, int height) {
    if (height == 0) height = 1;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (width >= height) {
        l = -1.0 * aspect;
        r = 1.0 * aspect;
        b = -1.0;
        t = 1.0;
    }
    else {
        l = -1.0;
        r = 1.0;
        b = -1.0 / aspect;
        t = 1.0 / aspect;
    }
    gluOrtho2D(l, r, b, t);
    BXMAX = r - (rd);
    BXMIN = l + (rd);
    BYMAX = t - (rd)-0.1;
    BYMIN = b + (rd);
    TBXMAX = r - (0.3);
    TBXMIN = l + (0.3);
}

void specialKeys(int key, int x, int y) {

    switch (key) {
        // Moving the board to left
    case GLUT_KEY_LEFT: if (flag == 0) {
        if (xt <= TBXMIN)
        {
            xt = TBXMIN;
        }
        else
        {
            xt = xt - 0.1;
        }
    }
                      break;
    case GLUT_KEY_RIGHT: if (flag == 0) {
        // Moving the board to right
        if (xt >= TBXMAX)
        {
            xt = TBXMAX;
        }
        else
        {
            xt = xt + 0.1;
        }
    }
                       break;
    }
}

int main(int argc, char** argv) {
    PlaySoundA("Data\\Music\\Background.wav", NULL, SND_ASYNC | SND_LOOP);   // Play Audio
    glutInit(&argc, argv);     // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE);   // Setting display mode
    glutInitWindowSize(840, 680);   // Set the window's initial width & height // Default value : 300,300
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutCreateWindow("ball");    // Create a window with the given title
    glutReshapeFunc(reshape);   // Register callback for window resize
    glutDisplayFunc(display);   // Register display callback handler for window re-paint
    glutSpecialFunc(specialKeys);   // Register keyboard callback
    glutIdleFunc(idle);     // Rigister timer callback
    initGL();   // Initialize GL
    glutMainLoop();   // Enter the event-processing loop
    return 0;
}
