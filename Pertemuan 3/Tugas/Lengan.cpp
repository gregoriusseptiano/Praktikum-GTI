/* Nama File    : Lengan,cpp
 * Deskripsi    : berisi program utama untuk membuat Lengan Bergerak
 * Pembuat      : Gregorius Septiano Ariadi
 * Tanggal      : Selasa, 17 Maret 2026
 */

#include <GL/glut.h>
#include <stdlib.h>

static int shoulder = 0, elbow = 0;

// Jari A, B, C
// Jari 1
int f1A = 0, f1B = 0, f1C = 0;
// Jari 2
int f2A = 0, f2B = 0, f2C = 0;
// Jari 3
int f3A = 0, f3B = 0, f3C = 0;
// Jari 4
int f4A = 0, f4B = 0, f4C = 0;
// Ibu Jari 
int f5A = 0, f5B = 0;

void init(void) {
    glClearColor(0, 0, 0, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

// Jari 3 Ruas
void drawFinger(float y, int A, int B, int C)
{
    glPushMatrix();
    glTranslatef(1.0, y, 0);

    // A
    glRotatef(A, 0, 0, 1);
    glPushMatrix();
    glScalef(0.5, 0.1, 0.1);
    glutWireCube(1);
    glPopMatrix();

    // B
    glTranslatef(0.5, 0, 0);
    glRotatef(B, 0, 0, 1);
    glPushMatrix();
    glScalef(0.4, 0.1, 0.1);
    glutWireCube(1);
    glPopMatrix();

    // C
    glTranslatef(0.4, 0, 0);
    glRotatef(C, 0, 0, 1);
    glPushMatrix();
    glScalef(0.3, 0.1, 0.1);
    glutWireCube(1);
    glPopMatrix();

    glPopMatrix();
}

// Ibu Jari
void drawThumb()
{
    glPushMatrix();
    glTranslatef(0.0, 0.8, 0);
    glRotatef(50, 0, 0, 1);

    // A
    glRotatef(f5A, 0, 0, 1);
    glPushMatrix();
    glScalef(0.4, 0.1, 0.1);
    glutWireCube(1);
    glPopMatrix();

    // B
    glTranslatef(0.4, 0, 0);
    glRotatef(f5B, 0, 0, 1);
    glPushMatrix();
    glScalef(0.3, 0.1, 0.1);
    glutWireCube(1);
    glPopMatrix();

    glPopMatrix();
}

// Display
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
        glTranslatef(-1.0, 0, 0);
        glRotatef((GLfloat)shoulder, 0, 0, 1);
        glTranslatef(1.0, 0, 0);

    // Bagian Bahu
    glPushMatrix();
        glScalef(2, 0.4 ,1);
        glutWireCube(1);
    glPopMatrix();

    // Bagian Sikut
    glTranslatef(1, 0, 0);
    glRotatef((GLfloat)elbow, 0, 0, 1);
    glTranslatef(1, 0, 0);

    glPushMatrix();
        glScalef(2, 0.4, 1);
        glutWireCube(1);
    glPopMatrix();
    glTranslatef(1.1, 0, 0);

    glPushMatrix();
        glScalef(1.5, 1.5, 0.5);
        glutWireCube(1);
    glPopMatrix();

    // Jari
    drawFinger(0.5, f1A, f1B, f1C);
    drawFinger(0.2, f2A, f2B, f2C);
    drawFinger(-0.2, f3A, f3B, f3C);
    drawFinger(-0.5, f4A, f4B, f4C);

    // Ibu Jari
    drawThumb();
    glPopMatrix();

    glutSwapBuffers();
}

// Reshape
void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65, (GLfloat)w / (GLfloat)h, 1, 20);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-1.8, 0, -6);
}

// Keyboard
void keyboard(unsigned char key, int x, int y) 
{
    switch(key) 
    {
    // Lengan
    case 's': 
        shoulder = (shoulder + 5) % 360; 
        break;
    case 'S': 
        shoulder = (shoulder - 5 + 360) % 360; 
        break;
    case 'e': 
        elbow = (elbow + 5) % 360; 
        break;
    case 'E': 
        elbow = (elbow - 5 + 360) % 360; 
        break;

    // Jari 1
    case 'q': 
        if (f1A < 90) f1A += 5; 
        break;
    case 'Q': 
        if (f1A > 0) f1A -= 5; 
        break;
    case 'w': 
        if (f1B < 90) f1B += 5; 
        break;
    case 'W': 
        if (f1B > 0) f1B -= 5; 
        break;
    case 'r': 
        if (f1C < 90) f1C += 5; 
        break;
    case 'R': 
        if (f1C > 0) f1C -= 5; 
        break;

    // Jari 2
    case 'a': 
        if (f2A < 90) f2A += 5; 
        break;
    case 'A': 
        if (f2A > 0) f2A -= 5; 
        break;
    case 'd': 
        if (f2B < 90) f2B += 5; 
        break;
    case 'D': 
        if (f2B > 0) f2B -= 5; 
        break;
    case 'f': 
        if (f2C < 90) f2C += 5; 
        break;
    case 'F': 
        if (f2C > 0) f2C -= 5; 
        break;

    // Jari 3
    case 'z': 
        if (f3A < 90) f3A += 5; 
        break;
    case 'Z': 
        if (f3A > 0) f3A -= 5; 
        break;
    case 'x': 
        if (f3B < 90) f3B += 5; 
        break;
    case 'X': 
        if (f3B > 0) f3B -= 5; 
        break;
    case 'c': 
        if (f3C < 90) f3C += 5; 
        break;
    case 'C': 
        if (f3C > 0) f3C -= 5; 
        break;

    // Jari 4
    case 'u': 
        if(f4A < 90) f4A += 5; 
        break;
    case 'U': 
        if(f4A > 0) f4A -= 5; 
        break;
    case 'i': 
        if(f4B < 90) f4B += 5; 
        break;
    case 'I': 
        if(f4B > 0) f4B -= 5; 
        break;
    case 'o': 
        if(f4C < 90) f4C += 5; 
        break;
    case 'O': 
        if(f4C > 0) f4C -= 5; 
        break;

    // Ibu Jari
    case 'k': 
        if(f5A < 90) f5A += 5; 
        break;
    case 'K': 
        if(f5A > 0) f5A -= 5; 
        break;
    case 'l': 
        if(f5B < 90) f5B += 5; 
        break;
    case 'L': 
        if(f5B > 0) f5B -= 5; 
        break;

    // Genggam Semua Jari
    case 'g':
        f1A = f1B = f1C = 60;
        f2A = f2B = f2C = 60;
        f3A = f3B = f3C = 60;
        f4A = f4B = f4C = 60;
        f5A = f5B = 60;
        break;

    case 27: 
        exit(0);
        break; 
    }

    glutPostRedisplay();
}

// Main
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);

    glutInitWindowSize(800, 600);
    glutCreateWindow("Lengan Bergerak");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}