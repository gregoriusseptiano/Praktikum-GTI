/*
 * ============================================================
 *  BALDI'S BASICS 3D - Main Entry Point & GLUT Callbacks
 *
 * COMPILE:
 *   Linux/Mac:
 *     g++ -o baldis3d main.cpp game.cpp -lGL -lGLU -lglut -lm
 *   Windows (MinGW):
 *     g++ -o baldis3d main.cpp game.cpp -lfreeglut -lopengl32 -lglu32 -lm
 *
 * CONTROLS:
 *   W/A/S/D or Arrow Keys  - Move
 *   Mouse                  - Look around
 *   E or Space             - Interact / Pick up
 *   B                      - Use B-Soda
 *   V                      - Toggle First / Third Person
 *   Shift                  - Sprint
 *   ESC                    - Pause / Quit
 * ============================================================
 */

#include "game.h"

/* ============================================================
 *  KEY STATES
 * ============================================================ */
int kW=0, kA=0, kS=0, kD=0, kShift=0, kE=0, kSpace=0;
int kUp=0, kDown=0, kLeft=0, kRight=0;
int kB=0;

/* ============================================================
 *  GLUT CALLBACKS
 * ============================================================ */
static int gLastTime = 0;

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (gState) {
    case STATE_PLAYING:
    case STATE_PAUSED:
        setupCamera();
        renderFloorAndCeiling();
        renderWalls();
        for (int i = 0; i < MAX_NOTEBOOKS; i++)
            renderNotebook(&gNotebooks[i]);
        renderItems();
        renderPlayer();
        renderBaldi();
        renderCloud();
        drawHUD();
        if (gShowQuestion) drawQuestion();
        if (gState == STATE_PAUSED) drawPause();
        break;
    default:
        break;
    }

    /* Overlay screens */
    switch (gState) {
    case STATE_MENU:         drawMenu();         break;
    case STATE_INSTRUCTIONS: drawInstructions(); break;
    case STATE_WIN:          drawWin();          break;
    case STATE_LOSE:         drawLose();         break;
    default: break;
    }

    glutSwapBuffers();
}

static void idle(void) {
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = (now - gLastTime) / 1000.0f;
    if (dt > 0.05f) dt = 0.05f;
    gLastTime = now;

    if (gState == STATE_PLAYING) {
        gTime += dt;
        updatePlayer(dt);
        updateBaldi(dt);

        if (gCloud.active) {
            gCloud.timeLeft -= dt;
            gCloud.radius   += dt * 0.5f;
            if (gCloud.timeLeft <= 0 || gCloud.radius > 4.0f)
                gCloud.active = 0;
        }
    } else if (gState == STATE_MENU || gState == STATE_WIN || gState == STATE_LOSE) {
        gTime += dt;
    }

    glutPostRedisplay();
}

static void reshape(int w, int h) {
    gWindowW = w;
    gWindowH = h ? h : 1;
    glViewport(0, 0, gWindowW, gWindowH);
}

static void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
    case 'w': case 'W': kW = 1; break;
    case 'a': case 'A': kA = 1; break;
    case 's': case 'S': kS = 1; break;
    case 'd': case 'D': kD = 1; break;
    case 'e': case 'E': kE = 1; break;
    case ' ':           kSpace = 1; break;

    case 'v': case 'V':
        if (gState == STATE_PLAYING) {
            gFirstPerson = !gFirstPerson;
            snprintf(gHudMsg, sizeof(gHudMsg),
                     gFirstPerson ? "Mode: First Person" : "Mode: Third Person");
            gHudMsgTimer = 2.0f;
        }
        break;

    case 'b': case 'B':
        if (gState == STATE_PLAYING && gPlayer.bsodaCount > 0 && !gCloud.active) {
            gPlayer.bsodaCount--;
            float yaw_r = gPlayer.yaw * 3.14159f / 180.0f;
            gCloud.x        = gPlayer.x + cosf(yaw_r) * 1.5f;
            gCloud.z        = gPlayer.z + sinf(yaw_r) * 1.5f;
            gCloud.active   = 1;
            gCloud.radius   = 0.5f;
            gCloud.timeLeft = 5.0f;
            snprintf(gHudMsg, sizeof(gHudMsg), "B-Soda dilempar!");
            gHudMsgTimer = 2.0f;
        }
        break;

    case 27: /* ESC */
        if (gState == STATE_PLAYING) {
            gState = STATE_PAUSED;
            glutSetCursor(GLUT_CURSOR_INHERIT);
            gMouseCaptured = 0;
        } else if (gState == STATE_PAUSED) {
            gState = STATE_PLAYING;
            glutSetCursor(GLUT_CURSOR_NONE);
            gMouseCaptured = 1;
            glutWarpPointer(gWindowW/2, gWindowH/2);
        } else if (gState == STATE_INSTRUCTIONS) {
            gState = STATE_MENU;
        } else if (gState == STATE_MENU) {
            exit(0);
        }
        break;

    case 13: /* Enter */
        if (gState == STATE_MENU) {
            if (gMenuSel == 0) {
                initGame();
                glutSetCursor(GLUT_CURSOR_NONE);
                gMouseCaptured = 1;
            } else if (gMenuSel == 1) {
                gState = STATE_INSTRUCTIONS;
            } else {
                exit(0);
            }
        } else if (gState == STATE_WIN || gState == STATE_LOSE) {
            initGame();
            glutSetCursor(GLUT_CURSOR_NONE);
            gMouseCaptured = 1;
        } else if (gState == STATE_PAUSED) {
            gState = STATE_MENU;
            glutSetCursor(GLUT_CURSOR_INHERIT);
            gMouseCaptured = 0;
        }
        break;

    /* Answer question */
    case '1': if (gShowQuestion) answerQuestion(1); break;
    case '2': if (gShowQuestion) answerQuestion(2); break;
    case '3': if (gShowQuestion) answerQuestion(3); break;
    }
    kShift = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? 1 : 0;
}

static void keyboardUp(unsigned char key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
    case 'w': case 'W': kW = 0; break;
    case 'a': case 'A': kA = 0; break;
    case 's': case 'S': kS = 0; break;
    case 'd': case 'D': kD = 0; break;
    case 'e': case 'E': kE = 0; break;
    case ' ':           kSpace = 0; break;
    }
    kShift = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? 1 : 0;
}

static void specialKey(int key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
    case GLUT_KEY_UP:
        if (gState == STATE_MENU) gMenuSel = (gMenuSel + 2) % 3;
        else kUp = 1;
        break;
    case GLUT_KEY_DOWN:
        if (gState == STATE_MENU) gMenuSel = (gMenuSel + 1) % 3;
        else kDown = 1;
        break;
    case GLUT_KEY_LEFT:  kLeft  = 1; break;
    case GLUT_KEY_RIGHT: kRight = 1; break;
    }
    kShift = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? 1 : 0;
}

static void specialKeyUp(int key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
    case GLUT_KEY_UP:    kUp    = 0; break;
    case GLUT_KEY_DOWN:  kDown  = 0; break;
    case GLUT_KEY_LEFT:  kLeft  = 0; break;
    case GLUT_KEY_RIGHT: kRight = 0; break;
    }
    kShift = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? 1 : 0;
}

static void mouseMotion(int x, int y) {
    if (!gMouseCaptured || gState != STATE_PLAYING) return;
    int dx = x - gWindowW/2;
    int dy = y - gWindowH/2;
    if (dx == 0 && dy == 0) return;

    gPlayer.yaw   += dx * MOUSE_SENS;
    gPlayer.pitch -= dy * MOUSE_SENS;
    gPlayer.pitch  = clampf(gPlayer.pitch, -70, 70);
    glutWarpPointer(gWindowW/2, gWindowH/2);
}

static void mouseClick(int button, int state, int x, int y) {
    (void)x; (void)y;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (gState == STATE_PLAYING && !gMouseCaptured) {
            glutSetCursor(GLUT_CURSOR_NONE);
            gMouseCaptured = 1;
            glutWarpPointer(gWindowW/2, gWindowH/2);
        }
    }
}

/* ============================================================
 *  MAIN
 * ============================================================ */
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_W, WINDOW_H);
    glutCreateWindow("Baldi's Basics 3D");

    /* OpenGL setup */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    /* Fog tipis — deket player terang normal, lorong jauh sedikit gelap.
       Mulai dari 6 unit, gelap penuh di 22 unit. Efek subtle, tidak dramatis. */
    glEnable(GL_FOG);
    GLfloat fogColor[4] = {0.06f, 0.06f, 0.08f, 1.0f};
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_START,   6.0f);
    glFogf(GL_FOG_END,    22.0f);
    glFogi(GL_FOG_MODE, GL_LINEAR);

    /* Lighting normal persis seperti aslinya */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat ambient[]  = {0.50f, 0.47f, 0.43f, 1.0f};
    GLfloat diffuse[]  = {0.65f, 0.62f, 0.58f, 1.0f};
    GLfloat specular[] = {0.0f,  0.0f,  0.0f,  1.0f};
    GLfloat lightPos[] = {0.0f,  1.0f,  0.0f,  0.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    gLastTime = glutGet(GLUT_ELAPSED_TIME);

    /* Register callbacks */
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKey);
    glutSpecialUpFunc(specialKeyUp);
    glutPassiveMotionFunc(mouseMotion);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouseClick);

    printf("=== BALDI'S BASICS 3D ===\n");
    printf("Compile: g++ -o baldis3d main.cpp game.cpp -lGL -lGLU -lglut -lm\n");
    printf("Klik window untuk aktifkan mouse, ESC untuk pause\n");

    glutMainLoop();
    return 0;
}