#include <gl/glut.h>
void Segitiga(void)
{
    glTranslatef( 0.25, -0.25, 0);
    glRotated(60.0, 0.0, 0.0, 1.0);
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex3f( -0.05,-0.05, 0.00);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex3f( 0.15,-0.05, 0.00);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex3f( -0.05, 0.05, 0.00);
    glEnd();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA); 
    glutCreateWindow("Segi Empat");
    glutDisplayFunc(Segitiga);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); 
    glutMainLoop();
    return 0;
}
