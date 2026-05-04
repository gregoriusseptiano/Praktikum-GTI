#include <GL/glut.h>
#include <stdlib.h>

float eyeX = 0.0f, eyeY = 0.0f, eyeZ = 5.0f;   // posisi kamera
float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f; // titik yang dilihat
float upX = 0.0f, upY = 1.0f, upZ = 0.0f;      // arah atas

void init()
{
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
}

void drawAxis()
{
    glBegin(GL_LINES);

    // X (merah)
    glColor3f(1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(5,0,0);

    // Y (hijau)
    glColor3f(0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,5,0);

    // Z (biru)
    glColor3f(0,0,1);
    glVertex3f(0,0,0);
    glVertex3f(0,0,5);

    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
        eyeX, eyeY, eyeZ,
        centerX, centerY, centerZ,
        upX, upY, upZ
    );

    drawAxis();

    // objek utama
    glColor3f(1, 0.5, 0);
    glutSolidTeapot(1.0);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;
    float ratio = (float)w / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60, ratio, 1, 100);

    glViewport(0, 0, w, h);
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        // posisi kamera
        case 'w': eyeZ -= 1; break;
        case 's': eyeZ += 1; break;
        case 'a': eyeX -= 1; break;
        case 'd': eyeX += 1; break;
        case 'r': eyeY += 1; break;
        case 'f': eyeY -= 1; break;

        //arah pandang
        case 'i': centerZ -= 1; break;
        case 'k': centerZ += 1; break;
        case 'j': centerX -= 1; break;
        case 'l': centerX += 1; break;
        case 'o': centerY -= 1; break;
        case 'p': centerY += 1; break;

        // reset
        case '0':
            eyeX = 0; eyeY = 0; eyeZ = 5;
            centerX = 0; centerY = 0; centerZ = 0;
            break;

        case 27: exit(0);
    }

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(800,600);
    glutCreateWindow("Simulasi gluLookAt");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}