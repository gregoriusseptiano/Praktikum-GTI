#include <GL/glut.h>

void Garis(void) {
    // Membersihkan layar
    glClear(GL_COLOR_BUFFER_BIT);

    glLineWidth(2.0f);            // Menentukan ketebalan garis
    glBegin(GL_LINES);            // Mulai menggambar garis
        glColor3f(1.0f, 1.0f, 1.0f); // Warna Putih (R=1, G=1, B=1)
        glVertex3f(0.00, 0.20, 0.0); // Titik awal (atas)
        glVertex3f(0.00, -0.20, 0.0);// Titik akhir (bawah)
    glEnd();

    glFlush();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Membuat Garis");

    glutDisplayFunc(Garis);

    // Warna latar belakang (Biru)
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    glutMainLoop();
    return 0;
}
