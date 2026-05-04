#include <GL/glut.h>
#include <stdlib.h>

// status lighting
bool lightingOn = false;
bool ambOn = false;
bool difOn = false;
bool speOn = false;

void updateLight()
{
    GLfloat amb[] = { ambOn ? 0.2f : 0.0f,
                      ambOn ? 0.2f : 0.0f,
                      ambOn ? 0.2f : 0.0f,
                      1.0f };

    GLfloat dif[] = { difOn ? 1.0f : 0.0f,
                      difOn ? 1.0f : 0.0f,
                      difOn ? 1.0f : 0.0f,
                      1.0f };

    GLfloat spe[] = { speOn ? 1.0f : 0.0f,
                      speOn ? 1.0f : 0.0f,
                      speOn ? 1.0f : 0.0f,
                      1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spe);
}

void init()
{
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHT0);

    GLfloat light_pos[] = {2.0f, 2.0f, 2.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    // material supaya specular kelihatan
    GLfloat mat_spec[] = {1.0f,1.0f,1.0f,1.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

    glEnable(GL_COLOR_MATERIAL);

    updateLight();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0,0,5, 0,0,0, 0,1,0);

    if (lightingOn)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);

    glColor3f(0.0f, 1.0f, 0.0f);
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
        case 'w': case 'W':
            lightingOn = !lightingOn;
            break;

        case 'a': case 'A':
            ambOn = !ambOn;
            break;

        case 'd': case 'D':
            difOn = !difOn;
            break;

        case 's': case 'S':
            speOn = !speOn;
            break;

        case 27:
            exit(0);
    }

    updateLight();
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(800,600);
    glutCreateWindow("Lighting Teko");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}