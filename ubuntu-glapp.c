/*
 * =====================================================================================
 *
 *       Filename:  ubunto-glapp.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Wednesday 19 December 2018 12:36:11  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish22@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include "GL/freeglut.h"
#include "GL/gl.h"

/* display function - code from:
     http://fly.cc.fer.hr/~unreal/theredbook/chapter01.html
This is the actual usage of the OpenGL library.
The following code is the same for any platform */
void renderFunction()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0, 0.0, 0.0);
        glVertex2f(-0.5, -0.5);
        glColor3f(0.0, 1.0, 0.0);
        glVertex2f(-0.5, 0.5);
        glColor3f(0.0, 0.0, 1.0);
        glVertex2f(0.5, 0.5);
        glColor3f(1.0, 0.0, 0.0);
        glVertex2f(0.5, -0.5);
    glEnd();
    glFlush();
}

/* Main method - main entry point of application
the freeglut library does the window creation work for us,
regardless of the platform. */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    glutCreateWindow("OpenGL - First window demo");
    glutDisplayFunc(renderFunction);
    glutMainLoop();
    return 0;
}

