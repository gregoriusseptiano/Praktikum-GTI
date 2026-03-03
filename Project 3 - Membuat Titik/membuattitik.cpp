#include <GL/glut.h>

void point(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glPointSize(5.0f); // Menentukan ukuran titik
    glBegin(GL_POINTS);
        glColor3f(1.0f, 0.0f, 0.0f);   // Warna titik: Merah
        glVertex3f(0.25, 0.25, 0.0);   // Posisi titik (x, y, z)
    glEnd();
    
    glFlush();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Titik");
    
    glutDisplayFunc(point);
    
    // Warna latar belakang: Biru
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    
    glutMainLoop();
    
    return 0;
}
