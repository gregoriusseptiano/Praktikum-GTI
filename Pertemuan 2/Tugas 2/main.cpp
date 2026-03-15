#include <GL/glut.h>
#include <math.h>

float wheelRotate = 0;
void drawCircle(float r)
{
    glBegin(GL_POLYGON);
    for(int i=0;i<100;i++)
    {
        float angle = 2 * 3.1416 * i / 100;
        float x = r * cos(angle);
        float y = r * sin(angle);
        glVertex2f(x,y);
    }
    glEnd();
}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    
	// Jalan
    glColor3f(0.1f,0.1f,0.1f);
    glBegin(GL_POLYGON);
        glVertex2f(-1,-0.5);
        glVertex2f(1,-0.5);
        glVertex2f(1,-1);
        glVertex2f(-1,-1);
    glEnd();

	// Mobil
    glPushMatrix();
    glTranslatef(0,-0.4,0);
    
    // Badan Mobil
    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
        glVertex2f(-0.45,0);
        glVertex2f(0.45,0);
        glVertex2f(0.45,0.2);
        glVertex2f(-0.45,0.2);
    glEnd();
    
    // Atap Mobil
    glBegin(GL_POLYGON);
        glVertex2f(-0.25,0.2);
        glVertex2f(0.25,0.2);
        glVertex2f(0.15,0.35);
        glVertex2f(-0.15,0.35);
    glEnd();
    
	// Jendela
    glColor3f(0.6,0.9,1);
    glBegin(GL_POLYGON);
        glVertex2f(-0.2,0.22);
        glVertex2f(0.2,0.22);
        glVertex2f(0.12,0.32);
        glVertex2f(-0.12,0.32);
    glEnd();
    
	// Kaca Depan
    glBegin(GL_POLYGON);
        glVertex2f(0.2,0.22);
        glVertex2f(0.12,0.32);
        glVertex2f(0.2,0.32);
        glVertex2f(0.28,0.22);
    glEnd();
    
	// Pintu Mobil
    glColor3f(0.8,0,0);
    glBegin(GL_POLYGON);
        glVertex2f(-0.1,0);
        glVertex2f(0.15,0);
        glVertex2f(0.15,0.18);
        glVertex2f(-0.1,0.18);
    glEnd();
    
	// Gagang Pintu
    glColor3f(0.9,0.9,0.9);
    glBegin(GL_POLYGON);
        glVertex2f(0.05,0.1);
        glVertex2f(0.1,0.1);
        glVertex2f(0.1,0.12);
        glVertex2f(0.05,0.12);
    glEnd();
    
	// Lampu Depan
    glColor3f(1,1,0);
    glBegin(GL_POLYGON);
        glVertex2f(0.45,0.05);
        glVertex2f(0.5,0.05);
        glVertex2f(0.5,0.12);
        glVertex2f(0.45,0.12);
    glEnd();
    
	// Lampu Belakang
    glColor3f(1,0.3,0);
    glBegin(GL_POLYGON);
        glVertex2f(-0.5,0.05);
        glVertex2f(-0.45,0.05);
        glVertex2f(-0.45,0.12);
        glVertex2f(-0.5,0.12);
    glEnd();
    
	// Roda Kiri
    glPushMatrix();
    glTranslatef(-0.28,-0.05,0);
    glRotatef(wheelRotate,0,0,1);
    glColor3f(0,0,0);
    drawCircle(0.08);

    // Velg
    glColor3f(0.8,0.8,0.8);
    drawCircle(0.04);
    glPopMatrix();

	// Roda Kanan
    glPushMatrix();
    glTranslatef(0.28,-0.05,0);
    glRotatef(wheelRotate,0,0,1);
    glColor3f(0,0,0);
    drawCircle(0.08);
    glColor3f(0.8,0.8,0.8);
    drawCircle(0.04);
    glPopMatrix();
    glPopMatrix();
    glFlush();
}

void timer(int value)
{
    wheelRotate -= 5;
    glutPostRedisplay();
    glutTimerFunc(30,timer,0);
}

void SetupRC()
{
    glClearColor(0.5,0.8,1,1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1,1,-1,1);
}

int main(int argc,char* argv[])
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800,600);
    glutCreateWindow("Mobil 2D OpenGL");
    SetupRC();
    glutDisplayFunc(RenderScene);
    glutTimerFunc(30,timer,0);
    glutMainLoop();

    return 0;
}
