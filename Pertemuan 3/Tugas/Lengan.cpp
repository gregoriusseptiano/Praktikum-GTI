#include <GL/glut.h>

int shoulder = 0;
int elbow = 0;
int finger = 0;

void init()
{
    glClearColor(0,0,0,0);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
}

void drawFinger(float y)
{
    glPushMatrix();

    glTranslatef(1.3,y,0);
    glRotatef(finger,0,0,1);

    for(int i=0;i<4;i++)
    {
        glPushMatrix();

        glTranslatef(i*0.45,0,0);
        glScalef(0.45,0.12,0.12);

        glutWireCube(1);

        glPopMatrix();
    }

    glPopMatrix();
}

void drawThumb()
{
    glPushMatrix();

    glTranslatef(0.0,1.4,0);
    glRotatef(90,0,0,1);
    glRotatef(finger,0,0,1);

    for(int i=0;i<4;i++)
    {
        glPushMatrix();

        glTranslatef(i*0.4,0,0);
        glScalef(0.4,0.12,0.12);

        glutWireCube(1);

        glPopMatrix();
    }

    glPopMatrix();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();

    // bahu
    glTranslatef(-5,0,0);
    glRotatef(shoulder,0,0,1);
    glTranslatef(2.5,0,0);

    // lengan atas
    glPushMatrix();
    glScalef(5,0.5,0.5);
    glutWireCube(1);
    glPopMatrix();

    // siku
    glTranslatef(2.5,0,0);
    glRotatef(elbow,0,0,1);
    glTranslatef(2.5,0,0);

    // lengan bawah
    glPushMatrix();
    glScalef(5,0.5,0.5);
    glutWireCube(1);
    glPopMatrix();

    // telapak
    glTranslatef(2.8,0,0);

    glPushMatrix();
    glScalef(2,2,0.6);
    glutWireCube(1);
    glPopMatrix();

    // jari
    drawFinger(0.6);
    drawFinger(0.2);
    drawFinger(-0.2);
    drawFinger(-0.6);

    // ibu jari
    drawThumb();

    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int w,int h)
{
    glViewport(0,0,w,h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(65,(float)w/h,1,30);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0,0,-15);
}

void keyboard(unsigned char key,int x,int y)
{
    switch(key)
    {
        case 's': shoulder=(shoulder+5)%360; break;
        case 'S': shoulder=(shoulder-5)%360; break;

        case 'e': elbow=(elbow+5)%360; break;
        case 'E': elbow=(elbow-5)%360; break;

        case 'f': finger=(finger+5)%90; break;
        case 'F': finger=(finger-5)%90; break;

        case 27: exit(0);
    }

    glutPostRedisplay();
}

int main(int argc,char** argv)
{
    glutInit(&argc,argv);

    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(800,600);

    glutCreateWindow("Robot Hand");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    return 0;
}