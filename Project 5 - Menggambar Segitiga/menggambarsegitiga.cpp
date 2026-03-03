#include <GL/glut.h>

void Segitiga(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);       
        glColor3f(1.0f, 0.0f, 0.0f); // Sudut 1: Merah
        glVertex3f(-0.10, -0.10, 0.00);
        
        glColor3f(0.0f, 1.0f, 0.0f); // Sudut 2: Hijau
        glVertex3f(0.10, -0.10, 0.00);
        
        glColor3f(0.0f, 0.0f, 1.0f); // Sudut 3: Biru
        glVertex3f(0.00, 0.10, 0.00);
    glEnd();
    glFlush();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Membuat Segitiga");
    glutDisplayFunc(Segitiga);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Latar Biru
    glutMainLoop();
    return 0;
}
