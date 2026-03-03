#include <GL/glut.h>

void SegiEmpat() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);

    // Segitiga pertama
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.2f, -0.2f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.2f, -0.2f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.2f, 0.2f, 0.0f);

    // Segitiga kedua
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.2f, -0.2f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.2f, 0.2f, 0.0f);

    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-0.2f, 0.2f, 0.0f);

    glEnd();

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Segiempat dari Dua Segitiga");
    glutDisplayFunc(SegiEmpat);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glutMainLoop();
    return 0;
}
