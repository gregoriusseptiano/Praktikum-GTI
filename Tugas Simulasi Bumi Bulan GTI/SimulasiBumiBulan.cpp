#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

float earthRotation = 0;
float moonAngle = 0;

float orbitRadius = 2.0;

float stars[200][2];

void initStars()
{
    for(int i=0;i<200;i++)
    {
        stars[i][0] = ((rand()%100)/50.0f-1)*4;
        stars[i][1] = ((rand()%100)/50.0f-1)*4;
    }
}

void drawStars()
{
    glPointSize(2);

    glBegin(GL_POINTS);

    for(int i=0;i<200;i++)
    {
        glColor3f(1,1,1);
        glVertex2f(stars[i][0],stars[i][1]);
    }

    glEnd();
}

void drawCircle(float r)
{
    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(0,0);

    for(int i=0;i<=100;i++)
    {
        float a=2*M_PI*i/100;
        glVertex2f(cos(a)*r,sin(a)*r);
    }

    glEnd();
}

void drawOrbit()
{
    glColor3f(0.7,0.7,0.7);

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00FF); // pola garis putus-putus

    glBegin(GL_LINE_LOOP);

    for(int i=0;i<100;i++)
    {
        float a = 2*M_PI*i/100;
        glVertex2f(cos(a)*orbitRadius,
                   sin(a)*orbitRadius);
    }

    glEnd();

    glDisable(GL_LINE_STIPPLE);
}

void drawEarth()
{
    glPushMatrix();

    glRotatef(earthRotation,0,0,1);

    // setengah biru
    glColor3f(0.3,0.5,1);

    glBegin(GL_POLYGON);
    for(int i=0;i<=180;i++)
    {
        float a=i*M_PI/180;
        glVertex2f(cos(a)*0.5,sin(a)*0.5);
    }
    glEnd();

    // setengah hijau
    glColor3f(0.3,0.7,0.3);

    glBegin(GL_POLYGON);
    for(int i=180;i<=360;i++)
    {
        float a=i*M_PI/180;
        glVertex2f(cos(a)*0.5,sin(a)*0.5);
    }
    glEnd();

    // garis tengah
    glColor3f(1,1,1);

    glBegin(GL_LINES);
    glVertex2f(-0.5,0);
    glVertex2f(0.5,0);
    glEnd();

    glPopMatrix();
}

void drawMoon()
{
    glColor3f(0.85,0.85,0.85);

    drawCircle(0.15);

    glColor3f(0.5,0.5,0.5);

    glBegin(GL_LINES);
    glVertex2f(-0.15,0);
    glVertex2f(0.15,0);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawStars();

    drawOrbit();

    drawEarth();

    float moonX = orbitRadius * cos(moonAngle);
    float moonY = orbitRadius * sin(moonAngle);

    glPushMatrix();

        glTranslatef(moonX,moonY,0);

        drawMoon();

    glPopMatrix();

    glutSwapBuffers();
}

void update(int value)
{
    earthRotation += 0.05;

    moonAngle += 0.01;

    glutPostRedisplay();

    glutTimerFunc(16,update,0);
}

void init()
{
    glClearColor(0,0,0.08,1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(-3,3,-3,3);

    glMatrixMode(GL_MODELVIEW);

    initStars();
}

int main(int argc,char** argv)
{
    glutInit(&argc,argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(800,800);

    glutCreateWindow("Animasi Bumi dan Bulan");

    init();

    glutDisplayFunc(display);

    glutTimerFunc(0,update,0);

    glutMainLoop();

    return 0;
}
