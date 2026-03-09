#include <GL/glut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);
    glRectf(-0.50, 0.55, -0.20, 0.2);
    glRectf(-0.64, 0.18, -0.34, -0.18);
    glRectf(-0.33, 0.18, -0.03, -0.18);
    glRectf(-0.02, 0.18, 0.28, -0.18);
    glRectf(-0.19, 0.55, 0.11, 0.2);
    glFlush();
}

void reshape(int w,int h){
    glViewport(0,0,w,h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (w<=h)
        gluOrtho2D(-1,1,-1*(float)h/w,1*(float)h/w);
    else
        gluOrtho2D(-1*(float)w/h,1*(float)w/h,-1,1);

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(600, 600); 
    glutCreateWindow("Tugas Primitif: Kubus Bertingkat"); 
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glutDisplayFunc(display);
    glutReshapeFunc(reshape); 
    glutMainLoop();
    return 0;
}


