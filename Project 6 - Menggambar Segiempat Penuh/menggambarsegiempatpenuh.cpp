#include <gl/glut.h>

void SegiEmpat(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);  // Warna Merah
    glRectf(-0.18, 0.18, 0.18, -0.18); // Koordinat (x1, y1, x2, y2)
    glFlush();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Segi Empat");
    glutDisplayFunc(SegiEmpat);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Latar Biru
    glutMainLoop();
    return 0;
}
