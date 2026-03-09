#include  <GL/glut.h>
void Segitiga(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glRotated(90.0, 0.0, 0.0, 1.0);
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(0.0, 0.0, 0.0);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(0.15, 0.0, 0.00);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.0, 0.15, 0.00);
    glEnd();
    glFlush();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Segi Tiga");
    glutDisplayFunc(Segitiga);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glutMainLoop();
    return 0;
}
