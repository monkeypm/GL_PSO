#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>

#include "tworld.h"

float TFood::r = 10.0f;
int TFood::counter = 0;

void _Display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    TWorld::get()->render();

    glutSwapBuffers();
}

void _TimerFunction(int value){
    TWorld::get()->update();

    glutPostRedisplay();
    glutTimerFunc(40,_TimerFunction,1);
}

void _MouseFunction(int button, int state, int x, int y)
{
    // спрацювання на відпуск миші
    if (state != GLUT_UP) return;

    switch (button)
    {
    case GLUT_LEFT_BUTTON:
         new TPS( TVector2d(x - WRLD_W2, -(y - WRLD_H2)) );
        break;
    case GLUT_MIDDLE_BUTTON:
        break;
    case GLUT_RIGHT_BUTTON:
        TWorld::get()->newFood( TVector2d(x - WRLD_W2, -(y - WRLD_H2)) );
        break;
    };
}

void _KeyFunction(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'q' :
        case 'Q' :
            exit(0); break;
         break;
        case 's' :
        case 'S' :
            TWorld::get()->ShowStat(); break;
         break;
    }
}

int main(int argcp, char **argv)
{
    atexit([]() { delete TWorld::get(); });

    glutInit(&argcp, argv);
    glutInitWindowSize(WRLD_W, WRLD_H);
    glutInitWindowPosition(0, 0);

    glutCreateWindow("My OpenGL Application");

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

    glClearColor(0.1, 0.1, 0.1, 1);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
    gluOrtho2D(-WRLD_W2, WRLD_W2, -WRLD_H2, WRLD_H2);

    glEnable(GL_PROGRAM_POINT_SIZE);

    glutDisplayFunc(_Display);
    glutTimerFunc(40,_TimerFunction,1);
    glutMouseFunc(_MouseFunction);
    glutKeyboardFunc(_KeyFunction);

    for (int i(0); i < 1000; i++)
        new TPS( TVector2d(0, 0) );

    glutMainLoop();

    return 0;
}

