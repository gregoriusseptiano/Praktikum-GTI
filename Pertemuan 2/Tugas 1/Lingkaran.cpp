#include <GL/glut.h>
#include <math.h>

void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_POLYGON);

    for(int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);

        float x = r * cos(theta);
        float y = r * sin(theta);

        glVertex2f(x + cx, y + cy);
    }

    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0, 0.0, 1.0); // warna biru

    glPushMatrix();
    drawCircle(0.0, 0.0, 0.5, 100); // pusat (0,0), radius 0.5
    glPopMatrix();

    glFlush();
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0); // background putih
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1,1,-1,1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600,600);
    glutCreateWindow("Lingkaran OpenGL");

    init();
    glutDisplayFunc(display);

    glutMainLoop();
}
