/*
 * ============================================================
 *  BALDI'S BASICS 3D - Game Logic & Rendering
 * ============================================================
 */

#include "game.h"

/* ============================================================
 *  COLORS
 * ============================================================ */
const float COL_WALL_A[3]  = {0.85f, 0.80f, 0.70f};
const float COL_WALL_B[3]  = {0.75f, 0.70f, 0.60f};
const float COL_FLOOR[3]   = {0.50f, 0.45f, 0.40f};
const float COL_CEIL[3]    = {0.90f, 0.88f, 0.82f};
const float COL_NOTEBOOK[3]= {0.10f, 0.40f, 0.90f};
const float COL_BALDI[3]   = {0.95f, 0.75f, 0.40f};
const float COL_STAMINA[3] = {0.20f, 0.80f, 0.20f};

/* ============================================================
 *  MAP (1=wall, 0=open, 2=exit)
 * ============================================================ */
int map[MAP_H][MAP_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1},
    {1,0,1,0,0,0,1,1,0,1,1,0,0,0,1,1},
    {1,0,1,0,1,0,0,1,0,0,1,0,1,0,0,1},
    {1,0,0,0,1,1,0,0,0,1,0,0,1,0,1,1},
    {1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,0,1,0,0,1,0,1},
    {1,0,1,1,0,0,0,1,0,1,0,0,1,0,0,1},
    {1,0,0,1,0,1,0,0,0,0,0,1,0,0,1,1},
    {1,1,0,0,0,0,1,0,1,1,0,0,0,1,0,1},
    {1,0,0,1,1,0,0,0,0,0,0,1,1,0,0,1},
    {1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,1},
    {1,0,0,0,1,1,0,0,0,0,0,1,0,1,0,1},
    {1,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,0,1,0,1,0,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

/* ============================================================
 *  GLOBALS
 * ============================================================ */
GameState   gState        = STATE_MENU;
Player      gPlayer;
Baldi       gBaldi;
Notebook    gNotebooks[MAX_NOTEBOOKS];
Item        gItems[MAX_ITEMS];
BsodaCloud  gCloud        = {0,0,0,0,0};
int         gWindowW      = WINDOW_W;
int         gWindowH      = WINDOW_H;
float       gTime         = 0.0f;
int         gMenuSel      = 0;
float       gFlashTimer   = 0.0f;
int         gMouseCaptured= 0;

int  gShowQuestion  = 0;
int  gQuestionIdx   = -1;
int  gAnswerA, gAnswerB, gAnswerC, gCorrectAnswer;
char gQuestionText[64];

char  gHudMsg[128]  = "";
float gHudMsgTimer  = 0.0f;

/* ============================================================
 *  UTILITY
 * ============================================================ */
float randf(float lo, float hi) {
    return lo + (hi - lo) * ((float)rand() / RAND_MAX);
}
float dist2D(float ax, float az, float bx, float bz) {
    float dx = ax - bx, dz = az - bz;
    return sqrtf(dx*dx + dz*dz);
}
float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}
static void setColor3fv(const float *c) { glColor3f(c[0], c[1], c[2]); }
static void setColor4fv(const float *c) { glColor4f(c[0], c[1], c[2], c[3]); }

/* ============================================================
 *  MAP HELPERS
 * ============================================================ */
int mapAt(int col, int row) {
    if (col < 0 || col >= MAP_W || row < 0 || row >= MAP_H) return 1;
    return map[row][col];
}
int mapSolid(int col, int row) {
    return mapAt(col, row) == 1;
}
void worldToCell(float wx, float wz, int *col, int *row) {
    *col = (int)(wx / CELL_SIZE);
    *row = (int)(wz / CELL_SIZE);
}
void moveWithCollision(float *cx, float *cz, float dx, float dz, float r) {
    float nx = *cx + dx;
    float nz = *cz + dz;
    int col, row;

    worldToCell(nx, *cz, &col, &row);
    if (!mapSolid(col, row)) {
        worldToCell(nx + r, *cz, &col, &row); int cA = mapSolid(col, row);
        worldToCell(nx - r, *cz, &col, &row); int cB = mapSolid(col, row);
        if (!cA && !cB) *cx = nx;
    }
    worldToCell(*cx, nz, &col, &row);
    if (!mapSolid(col, row)) {
        worldToCell(*cx, nz + r, &col, &row); int cA = mapSolid(col, row);
        worldToCell(*cx, nz - r, &col, &row); int cB = mapSolid(col, row);
        if (!cA && !cB) *cz = nz;
    }
}

/* ============================================================
 *  QUESTION GENERATOR
 * ============================================================ */
static void generateQuestion(int notebookIdx) {
    int a = (rand() % 9) + 1, b = (rand() % 9) + 1;
    int ops = rand() % 3;
    if (ops == 0) {
        snprintf(gQuestionText, sizeof(gQuestionText), "%d + %d = ?", a, b);
        gCorrectAnswer = a + b;
    } else if (ops == 1) {
        if (a < b) { int t = a; a = b; b = t; }
        snprintf(gQuestionText, sizeof(gQuestionText), "%d - %d = ?", a, b);
        gCorrectAnswer = a - b;
    } else {
        snprintf(gQuestionText, sizeof(gQuestionText), "%d x %d = ?", a, b);
        gCorrectAnswer = a * b;
    }
    int pos = rand() % 3;
    int choices[3];
    choices[pos] = gCorrectAnswer;
    for (int i = 0; i < 3; i++) {
        if (i == pos) continue;
        int wrong;
        do { wrong = gCorrectAnswer + (rand() % 7) - 3; }
        while (wrong == gCorrectAnswer || wrong < 0);
        choices[i] = wrong;
    }
    gAnswerA = choices[0];
    gAnswerB = choices[1];
    gAnswerC = choices[2];
    gQuestionIdx  = notebookIdx;
    gShowQuestion = 1;
}

/* ============================================================
 *  GAME INIT
 * ============================================================ */
static void placeNotebooks(void) {
    int sx[] = {3, 7,11, 5,13, 2, 9};
    int sz[] = {2, 4, 6,10, 3,12,13};
    for (int i = 0; i < MAX_NOTEBOOKS; i++) {
        gNotebooks[i].x               = sx[i] * CELL_SIZE + CELL_SIZE * 0.5f;
        gNotebooks[i].z               = sz[i] * CELL_SIZE + CELL_SIZE * 0.5f;
        gNotebooks[i].collected       = 0;
        gNotebooks[i].active          = 1;
        gNotebooks[i].bobOffset       = randf(0, 6.28f);
        gNotebooks[i].questionAnswered = -1;
    }
}

static void placeItems(void) {
    int   sx[]    = {4, 9, 2,12, 7};
    int   sz[]    = {7, 9,11, 5,14};
    int   types[] = {0, 1, 2, 3, 4};
    for (int i = 0; i < MAX_ITEMS; i++) {
        gItems[i].x      = sx[i] * CELL_SIZE + CELL_SIZE * 0.5f;
        gItems[i].z      = sz[i] * CELL_SIZE + CELL_SIZE * 0.5f;
        gItems[i].type   = types[i];
        gItems[i].active = 1;
    }
}

void initGame(void) {
    srand((unsigned)time(NULL));

    gPlayer.x                  = 1.5f * CELL_SIZE;
    gPlayer.z                  = 1.5f * CELL_SIZE;
    gPlayer.yaw                = 0.0f;
    gPlayer.pitch              = 0.0f;
    gPlayer.stamina            = 1.0f;
    gPlayer.sprinting          = 0;
    gPlayer.notebooksCollected = 0;
    gPlayer.energyDrinks       = 0;
    gPlayer.hasRuler           = 0;
    gPlayer.bsodaCount         = 0;
    gPlayer.interactCooldown   = 0.0f;
    gPlayer.footstepTimer      = 0.0f;

    gBaldi.x                  = 13.5f * CELL_SIZE;
    gBaldi.z                  = 13.5f * CELL_SIZE;
    gBaldi.angle              = 180.0f;
    gBaldi.speed              = BALDI_SPEED_BASE;
    gBaldi.angry              = 0;
    gBaldi.angerTimer         = 0.0f;
    gBaldi.hearRadius         = 8.0f;

    placeNotebooks();
    placeItems();

    gCloud.active   = 0;
    gFlashTimer     = 0;
    gShowQuestion   = 0;
    snprintf(gHudMsg, sizeof(gHudMsg),
             "Kumpulkan semua %d buku catatan!", MAX_NOTEBOOKS);
    gHudMsgTimer = 4.0f;
    gTime        = 0.0f;
    gState       = STATE_PLAYING;
}

/* ============================================================
 *  ANSWER QUESTION
 * ============================================================ */
void answerQuestion(int choice) {
    int chosen = (choice == 1) ? gAnswerA : (choice == 2) ? gAnswerB : gAnswerC;

    if (chosen == gCorrectAnswer) {
        gNotebooks[gQuestionIdx].active    = 0;
        gNotebooks[gQuestionIdx].collected = 1;
        gPlayer.notebooksCollected++;
        snprintf(gHudMsg, sizeof(gHudMsg), "Benar! Buku %d/%d terkumpul!",
                 gPlayer.notebooksCollected, MAX_NOTEBOOKS);
        gHudMsgTimer      = 3.0f;
        gBaldi.hearRadius = 20.0f;
        gBaldi.speed      = BALDI_SPEED_BASE + gPlayer.notebooksCollected * 0.005f;
    } else {
        snprintf(gHudMsg, sizeof(gHudMsg), "SALAH! Baldi makin cepat!");
        gHudMsgTimer  = 3.0f;
        gFlashTimer   = 1.0f;
        gBaldi.angry++;
        if (gBaldi.angry > 2) gBaldi.angry = 2;
        gBaldi.angerTimer = 8.0f;
        gBaldi.speed = BALDI_SPEED_BASE
                     + gPlayer.notebooksCollected * 0.005f
                     + gBaldi.angry * 0.012f;
    }
    gShowQuestion = 0;
    gQuestionIdx  = -1;
}

/* ============================================================
 *  UPDATE: PLAYER
 * ============================================================ */
void updatePlayer(float dt) {
    if (gState != STATE_PLAYING) return;

    gPlayer.sprinting = (kShift && gPlayer.stamina > 0.0f) ? 1 : 0;
    float speed = MOVE_SPEED * (gPlayer.sprinting ? 1.6f : 1.0f);

    if (gPlayer.sprinting) {
        gPlayer.stamina -= dt * 0.35f;
        if (gPlayer.stamina < 0) gPlayer.stamina = 0;
    } else {
        gPlayer.stamina += dt * 0.25f;
        if (gPlayer.stamina > 1) gPlayer.stamina = 1;
    }

    float yaw_r = gPlayer.yaw * 3.14159f / 180.0f;
    float fx = cosf(yaw_r), fz = sinf(yaw_r);
    float rx = -fz,         rz = fx;

    if (kLeft)  gPlayer.yaw -= 120.0f * dt;
    if (kRight) gPlayer.yaw += 120.0f * dt;
    if (kUp)    gPlayer.pitch += 120.0f * dt;
    if (kDown)  gPlayer.pitch -= 120.0f * dt;
    gPlayer.pitch = clampf(gPlayer.pitch, -70, 70);

    float dx = 0, dz = 0;
    if (kW) { dx += fx; dz += fz; }
    if (kS) { dx -= fx; dz -= fz; }
    if (kA) { dx -= rx; dz -= rz; }
    if (kD) { dx += rx; dz += rz; }

    float len = sqrtf(dx*dx + dz*dz);
    if (len > 0) { dx /= len; dz /= len; }
    float frameScale = dt * TARGET_FPS;
    dx *= speed * frameScale;
    dz *= speed * frameScale;
    moveWithCollision(&gPlayer.x, &gPlayer.z, dx, dz, 0.25f);

    if (gPlayer.interactCooldown > 0) gPlayer.interactCooldown -= dt;

    /* Notebook / item pickup */
    if ((kE || kSpace) && !gShowQuestion && gPlayer.interactCooldown <= 0) {
        for (int i = 0; i < MAX_NOTEBOOKS; i++) {
            if (!gNotebooks[i].active) continue;
            float d = dist2D(gPlayer.x, gPlayer.z, gNotebooks[i].x, gNotebooks[i].z);
            if (d < 0.9f) {
                generateQuestion(i);
                gPlayer.interactCooldown = 0.5f;
                break;
            }
        }
        for (int i = 0; i < MAX_ITEMS; i++) {
            if (!gItems[i].active) continue;
            float d = dist2D(gPlayer.x, gPlayer.z, gItems[i].x, gItems[i].z);
            if (d < 0.9f) {
                gItems[i].active = 0;
                switch (gItems[i].type) {
                    case 0: gPlayer.energyDrinks++;
                            snprintf(gHudMsg, sizeof(gHudMsg),
                                     "Dapat Energy Drink! Tekan E untuk pakai"); break;
                    case 1: gPlayer.hasRuler = 1;
                            snprintf(gHudMsg, sizeof(gHudMsg),
                                     "Dapat Ruler! Tekan R untuk pakai"); break;
                    case 2: gPlayer.bsodaCount++;
                            snprintf(gHudMsg, sizeof(gHudMsg),
                                     "Dapat B-Soda! Tekan B untuk lempar ke Baldi"); break;
                    case 3: speed += 0.01f;
                            snprintf(gHudMsg, sizeof(gHudMsg),
                                     "Zesty Bar! Speed naik permanen!"); break;
                    case 4: snprintf(gHudMsg, sizeof(gHudMsg),
                                     "Principal Tape! Baldi diam sebentar!");
                            gBaldi.angerTimer = 0; gBaldi.angry = 0; break;
                }
                gHudMsgTimer = 3.0f;
                gPlayer.interactCooldown = 0.5f;
                break;
            }
        }
    }

    /* Use energy drink */
    static int prevE = 0;
    if (kE && !prevE && gPlayer.energyDrinks > 0) {
        gPlayer.energyDrinks--;
        gPlayer.stamina = 1.0f;
        snprintf(gHudMsg, sizeof(gHudMsg), "Stamina penuh!");
        gHudMsgTimer = 2.0f;
    }
    prevE = kE;

    /* Check exit */
    int col, row;
    worldToCell(gPlayer.x, gPlayer.z, &col, &row);
    if (mapAt(col, row) == 2 && gPlayer.notebooksCollected == MAX_NOTEBOOKS) {
        gState = STATE_WIN;
        if (gMouseCaptured) { glutSetCursor(GLUT_CURSOR_INHERIT); gMouseCaptured = 0; }
    } else if (mapAt(col, row) == 2 && gPlayer.notebooksCollected < MAX_NOTEBOOKS) {
        snprintf(gHudMsg, sizeof(gHudMsg), "Kumpulkan semua buku dulu! (%d/%d)",
                 gPlayer.notebooksCollected, MAX_NOTEBOOKS);
        gHudMsgTimer = 2.0f;
    }

    if (gFlashTimer  > 0) gFlashTimer  -= dt;
    if (gHudMsgTimer > 0) gHudMsgTimer -= dt;
}

/* ============================================================
 *  UPDATE: BALDI AI
 * ============================================================ */
void updateBaldi(float dt) {
    if (gState != STATE_PLAYING) return;

    if (gCloud.active) {
        float d = dist2D(gBaldi.x, gBaldi.z, gCloud.x, gCloud.z);
        if (d < gCloud.radius) return; /* stunned */
    }

    float dx = gPlayer.x - gBaldi.x;
    float dz = gPlayer.z - gBaldi.z;
    float dist = sqrtf(dx*dx + dz*dz);
    if (dist < 0.01f) return;

    float nx = dx / dist, nz = dz / dist;

    int canHear = (dist < gBaldi.hearRadius);
    if (gPlayer.sprinting) canHear = (dist < gBaldi.hearRadius * 1.5f);
    if (!canHear && gBaldi.angry == 0) return;

    float sp = gBaldi.speed;
    if (gBaldi.angry == 1) sp *= 1.3f;
    if (gBaldi.angry == 2) sp *= 1.7f;

    float frameScale = dt * TARGET_FPS;
    moveWithCollision(&gBaldi.x, &gBaldi.z, nx * sp * frameScale, nz * sp * frameScale, 0.3f);
    gBaldi.angle = atan2f(nz, nx) * 180.0f / 3.14159f + 90.0f;

    if (dist2D(gPlayer.x, gPlayer.z, gBaldi.x, gBaldi.z) < 0.55f) {
        gState = STATE_LOSE;
        if (gMouseCaptured) { glutSetCursor(GLUT_CURSOR_INHERIT); gMouseCaptured = 0; }
    }

    if (gBaldi.angerTimer > 0) {
        gBaldi.angerTimer -= dt;
        if (gBaldi.angerTimer <= 0) gBaldi.angry = 0;
    }
}

/* ============================================================
 *  DRAWING HELPERS (2D overlay)
 * ============================================================ */
static void drawText2D(float x, float y, const char *txt, void *font) {
    glRasterPos2f(x, y);
    for (int i = 0; txt[i]; i++) glutBitmapCharacter(font, txt[i]);
}

static void begin2D(void) {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, gWindowW, 0, gWindowH);
    glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
static void end2D(void) {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
}

static void drawRect2D(float x, float y, float w, float h, const float *col4) {
    setColor4fv(col4);
    glBegin(GL_QUADS);
    glVertex2f(x,     y);     glVertex2f(x+w, y);
    glVertex2f(x+w,   y+h);   glVertex2f(x,   y+h);
    glEnd();
}

/* ============================================================
 *  3D GEOMETRY HELPERS
 * ============================================================ */
static void drawBox(float x, float y, float z,
                    float sx, float sy, float sz, const float *col) {
    setColor3fv(col);
    float x0=x-sx/2, x1=x+sx/2, y0=y, y1=y+sy, z0=z-sz/2, z1=z+sz/2;
    glBegin(GL_QUADS);
    /* Top   */ glVertex3f(x0,y1,z0); glVertex3f(x1,y1,z0); glVertex3f(x1,y1,z1); glVertex3f(x0,y1,z1);
    /* Bot   */ glVertex3f(x0,y0,z1); glVertex3f(x1,y0,z1); glVertex3f(x1,y0,z0); glVertex3f(x0,y0,z0);
    /* Front */ glVertex3f(x0,y0,z1); glVertex3f(x1,y0,z1); glVertex3f(x1,y1,z1); glVertex3f(x0,y1,z1);
    /* Back  */ glVertex3f(x0,y1,z0); glVertex3f(x1,y1,z0); glVertex3f(x1,y0,z0); glVertex3f(x0,y0,z0);
    /* Left  */ glVertex3f(x0,y0,z0); glVertex3f(x0,y0,z1); glVertex3f(x0,y1,z1); glVertex3f(x0,y1,z0);
    /* Right */ glVertex3f(x1,y1,z0); glVertex3f(x1,y1,z1); glVertex3f(x1,y0,z1); glVertex3f(x1,y0,z0);
    glEnd();
}

static void drawWallTexture(float wx, float wz, const float *baseCol) {
    float x0=wx, x1=wx+CELL_SIZE, z0=wz, z1=wz+CELL_SIZE, eps=0.006f;
    float mortar[3] = {baseCol[0]*0.55f, baseCol[1]*0.55f, baseCol[2]*0.55f};
    float speck[3]  = {baseCol[0]*1.08f, baseCol[1]*1.08f, baseCol[2]*1.08f};

    glLineWidth(1.0f);
    setColor3fv(mortar);
    glBegin(GL_LINES);
    for (float yy = 0.45f; yy < WALL_HEIGHT; yy += 0.45f) {
        glVertex3f(x0,yy,z0-eps); glVertex3f(x1,yy,z0-eps);
        glVertex3f(x0,yy,z1+eps); glVertex3f(x1,yy,z1+eps);
        glVertex3f(x0-eps,yy,z0); glVertex3f(x0-eps,yy,z1);
        glVertex3f(x1+eps,yy,z0); glVertex3f(x1+eps,yy,z1);
    }
    for (float yy = 0.0f; yy < WALL_HEIGHT; yy += 0.45f) {
        float off = fmodf(yy/0.45f, 2.0f) < 1.0f ? 0.0f : 0.35f;
        for (float p = 0.35f+off; p < CELL_SIZE; p += 0.7f) {
            float yTop = yy+0.45f; if (yTop>WALL_HEIGHT) yTop=WALL_HEIGHT;
            glVertex3f(x0+p,yy,z0-eps); glVertex3f(x0+p,yTop,z0-eps);
            glVertex3f(x0+p,yy,z1+eps); glVertex3f(x0+p,yTop,z1+eps);
            glVertex3f(x0-eps,yy,z0+p); glVertex3f(x0-eps,yTop,z0+p);
            glVertex3f(x1+eps,yy,z0+p); glVertex3f(x1+eps,yTop,z0+p);
        }
    }
    glEnd();

    setColor3fv(speck);
    glBegin(GL_QUADS);
    for (int i = 0; i < 6; i++) {
        float px = x0+0.22f+fmodf((float)(i*37),140.0f)/100.0f;
        float pz = z0+0.18f+fmodf((float)(i*53),145.0f)/100.0f;
        float py = 0.35f+fmodf((float)(i*29),175.0f)/100.0f;
        float s  = 0.025f;
        if (px < x1-0.1f) {
            glVertex3f(px-s,py-s,z0-eps*1.5f); glVertex3f(px+s,py-s,z0-eps*1.5f);
            glVertex3f(px+s,py+s,z0-eps*1.5f); glVertex3f(px-s,py+s,z0-eps*1.5f);
        }
        if (pz < z1-0.1f) {
            glVertex3f(x1+eps*1.5f,py-s,pz-s); glVertex3f(x1+eps*1.5f,py-s,pz+s);
            glVertex3f(x1+eps*1.5f,py+s,pz+s); glVertex3f(x1+eps*1.5f,py+s,pz-s);
        }
    }
    glEnd();
}

/* ============================================================
 *  WORLD RENDERING
 * ============================================================ */
void setupCamera(void) {
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(FOV, (float)gWindowW/gWindowH, 0.05f, 100.0f);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();

    float yaw_r = gPlayer.yaw * 3.14159f / 180.0f;

    /* Third person: geser kamera ke belakang player,
       cek collision per 0.05 unit dari jauh ke dekat */
    float camDist  = 1.5f;
    float camHeight= 0.4f;
    float r        = 0.15f; /* radius sudut kamera */

    float finalDist = 0.3f;
    for (float d = camDist; d > 0.3f; d -= 0.05f) {
        float testX = gPlayer.x - cosf(yaw_r) * d;
        float testZ = gPlayer.z - sinf(yaw_r) * d;
        int col, row;
        worldToCell(testX + r, testZ + r, &col, &row); if (mapSolid(col, row)) continue;
        worldToCell(testX + r, testZ - r, &col, &row); if (mapSolid(col, row)) continue;
        worldToCell(testX - r, testZ + r, &col, &row); if (mapSolid(col, row)) continue;
        worldToCell(testX - r, testZ - r, &col, &row); if (mapSolid(col, row)) continue;
        finalDist = d;
        break;
    }

    float camX = gPlayer.x - cosf(yaw_r) * finalDist;
    float camY = 1.10f + camHeight;
    float camZ = gPlayer.z - sinf(yaw_r) * finalDist;

    float pitch_r = gPlayer.pitch * 3.14159f / 180.0f;

    // Target lihat = posisi player + offset pitch
    float targetX = gPlayer.x + cosf(yaw_r) * cosf(pitch_r) * 2.0f;
    float targetY = 0.8f      + sinf(pitch_r) * 2.0f;
    float targetZ = gPlayer.z + sinf(yaw_r) * cosf(pitch_r) * 2.0f;

    gluLookAt(camX, camY, camZ,
            targetX, targetY, targetZ,
            0, 1, 0);
}

void renderFloorAndCeiling(void) {
    glBegin(GL_QUADS);
    setColor3fv(COL_FLOOR);
    glVertex3f(0,0,0); glVertex3f(MAP_W*CELL_SIZE,0,0);
    glVertex3f(MAP_W*CELL_SIZE,0,MAP_H*CELL_SIZE); glVertex3f(0,0,MAP_H*CELL_SIZE);
    setColor3fv(COL_CEIL);
    glVertex3f(0,WALL_HEIGHT,MAP_H*CELL_SIZE); glVertex3f(MAP_W*CELL_SIZE,WALL_HEIGHT,MAP_H*CELL_SIZE);
    glVertex3f(MAP_W*CELL_SIZE,WALL_HEIGHT,0); glVertex3f(0,WALL_HEIGHT,0);
    glEnd();
}

void renderWalls(void) {
    for (int row = 0; row < MAP_H; row++) {
        for (int col = 0; col < MAP_W; col++) {
            int v = mapAt(col, row);
            if (v == 0) continue;
            float wx = col*CELL_SIZE, wz = row*CELL_SIZE;
            const float *wc = ((col+row)%2==0) ? COL_WALL_A : COL_WALL_B;

            if (v == 2) {
                float ec[3] = {0.10f,0.85f,0.20f};
                drawBox(wx+CELL_SIZE*0.5f,0,wz+CELL_SIZE*0.5f,CELL_SIZE,WALL_HEIGHT,CELL_SIZE,ec);
                float sc[3] = {0.9f,0.9f,0.1f};
                drawBox(wx+CELL_SIZE*0.5f,WALL_HEIGHT*0.55f,wz+CELL_SIZE*0.5f-0.05f,0.6f,0.35f,0.05f,sc);
            } else {
                drawBox(wx+CELL_SIZE*0.5f,0,wz+CELL_SIZE*0.5f,CELL_SIZE,WALL_HEIGHT,CELL_SIZE,wc);
                drawWallTexture(wx, wz, wc);
                float stripe[3] = {wc[0]*0.7f, wc[1]*0.7f, wc[2]*0.7f};
                drawBox(wx+CELL_SIZE*0.5f,WALL_HEIGHT-0.15f,wz+CELL_SIZE*0.5f,
                        CELL_SIZE+0.01f,0.15f,CELL_SIZE+0.01f,stripe);
            }
        }
    }
}

void renderNotebook(Notebook *nb) {
    if (!nb->active) return;
    float bob = sinf(gTime*2.0f + nb->bobOffset)*0.07f;
    float rot  = fmodf(gTime*30.0f, 360.0f);

    glPushMatrix();
    glTranslatef(nb->x, 0.4f+bob, nb->z);
    glRotatef(rot, 0,1,0);

    setColor3fv(COL_NOTEBOOK);
    glBegin(GL_QUADS);
    glVertex3f(-0.12f,0,-0.18f); glVertex3f(0.12f,0,-0.18f); glVertex3f(0.12f,0.30f,-0.18f); glVertex3f(-0.12f,0.30f,-0.18f);
    glVertex3f(-0.12f,0, 0.18f); glVertex3f(0.12f,0, 0.18f); glVertex3f(0.12f,0.30f, 0.18f); glVertex3f(-0.12f,0.30f, 0.18f);
    glVertex3f(-0.12f,0,-0.18f); glVertex3f(-0.12f,0, 0.18f); glVertex3f(-0.12f,0.30f,0.18f); glVertex3f(-0.12f,0.30f,-0.18f);
    glVertex3f( 0.12f,0.30f,-0.18f); glVertex3f(0.12f,0.30f,0.18f); glVertex3f(0.12f,0,0.18f); glVertex3f(0.12f,0,-0.18f);
    glEnd();

    glColor3f(0.95f,0.95f,0.92f);
    glBegin(GL_QUADS);
    glVertex3f(-0.10f,0.01f,-0.17f); glVertex3f(0.10f,0.01f,-0.17f);
    glVertex3f(0.10f,0.28f,-0.17f);  glVertex3f(-0.10f,0.28f,-0.17f);
    glEnd();

    glColor3f(0.5f,0.7f,1.0f);
    glBegin(GL_LINES);
    for (float ly=0.05f; ly<0.26f; ly+=0.05f) {
        glVertex3f(-0.09f,ly,-0.169f); glVertex3f(0.09f,ly,-0.169f);
    }
    glEnd();
    glPopMatrix();
}

void renderPlayer(void) {
    float yaw_r = gPlayer.yaw * 3.14159f / 180.0f;
    (void)yaw_r;
    glPushMatrix();
    glTranslatef(gPlayer.x, 0.0f, gPlayer.z);
    glRotatef(-gPlayer.yaw + 90.0f, 0,1,0);

    int isMoving = (kW || kS || kA || kD);
    float swing = isMoving ? sinf(gTime * 12.0f) : 0.0f;

    /* Paha kiri (diam) + betis kiri (menekuk) */
    glPushMatrix();
    glTranslatef(-0.08f, 0.48f, 0);
    drawBox(0, -0.24f, 0, 0.13f, 0.24f, 0.13f, (float[]){0.10f,0.10f,0.45f}); /* paha */
    glTranslatef(0, -0.24f, 0);
    glRotatef(swing * 40, 1,0,0); /* lutut menekuk */
    drawBox(0, -0.24f, 0, 0.12f, 0.24f, 0.12f, (float[]){0.10f,0.10f,0.45f}); /* betis */
    glPopMatrix();

    /* Paha kanan (diam) + betis kanan (menekuk berlawanan) */
    glPushMatrix();
    glTranslatef(0.08f, 0.48f, 0);
    drawBox(0, -0.24f, 0, 0.13f, 0.24f, 0.13f, (float[]){0.10f,0.10f,0.45f}); /* paha */
    glTranslatef(0, -0.24f, 0);
    glRotatef(-swing * 40, 1,0,0); /* lutut menekuk berlawanan */
    drawBox(0, -0.24f, 0, 0.12f, 0.24f, 0.12f, (float[]){0.10f,0.10f,0.45f}); /* betis */
    glPopMatrix();

    /* Badan */
    drawBox(0,0.48f,0,0.36f,0.52f,0.22f,(float[]){0.15f,0.45f,0.85f});

    /* Kepala */
        drawBox(0,1.10f,0,0.26f,0.26f,0.26f,(float[]){0.88f,0.72f,0.58f});

        /* Lengan kiri */
        glPushMatrix();
        glTranslatef(-0.25f, 0.48f, 0);
        glRotatef(20 + sinf(gTime*8.0f)*25, 1,0,0);
        drawBox(0,0,0,0.12f,0.45f,0.12f,(float[]){0.88f,0.72f,0.58f});
        glPopMatrix();

        /* Lengan kanan */
        glPushMatrix();
        glTranslatef(0.25f, 0.48f, 0);
        glRotatef(20 - sinf(gTime*8.0f)*25, 1,0,0);
        drawBox(0,0,0,0.12f,0.45f,0.12f,(float[]){0.88f,0.72f,0.58f});
        glPopMatrix();

        glPopMatrix();
    }

void renderBaldi(void) {
    float anger = (gBaldi.angry==2)?1.0f:(gBaldi.angry==1)?0.5f:0.0f;
    float headCol[3] = {
        COL_BALDI[0]*(1-anger)+0.95f*anger,
        COL_BALDI[1]*(1-anger)+0.20f*anger,
        COL_BALDI[2]*(1-anger)+0.10f*anger
    };
    float sway = sinf(gTime*8.0f)*0.03f;

    glPushMatrix();
    glTranslatef(gBaldi.x, 0, gBaldi.z);
    glRotatef(180.0f - gBaldi.angle, 0,1,0);

    float legSwing = sinf(gTime*8.0f)*0.15f;
    /* Left leg */
    glPushMatrix(); glTranslatef(-0.12f,0,sway); glRotatef(legSwing*30,1,0,0);
    drawBox(0,0,0,0.18f,0.55f,0.18f,(float[]){0.25f,0.25f,0.70f}); glPopMatrix();
    /* Right leg */
    glPushMatrix(); glTranslatef(0.12f,0,-sway); glRotatef(-legSwing*30,1,0,0);
    drawBox(0,0,0,0.18f,0.55f,0.18f,(float[]){0.25f,0.25f,0.70f}); glPopMatrix();

    drawBox(0,0.55f,0,0.45f,0.60f,0.30f,(float[]){0.90f,0.30f,0.10f});

    /* Ruler arm */
    glPushMatrix(); glTranslatef(0.30f,0.90f,0);
    glRotatef(-30+sinf(gTime*6)*20,1,0,0);
    drawBox(0,0,0,0.06f,0.55f,0.06f,(float[]){0.85f,0.65f,0.10f}); glPopMatrix();

    /* Left arm */
    glPushMatrix(); glTranslatef(-0.30f,0.90f,0);
    glRotatef(20+cosf(gTime*6)*15,1,0,0);
    drawBox(0,0,0,0.12f,0.45f,0.12f,(float[]){0.95f,0.75f,0.40f}); glPopMatrix();

    drawBox(0,1.25f,0,0.40f,0.40f,0.35f,headCol);

    /* Eyes */
    float eyeAnger = anger*0.05f;
    glColor3f(0.05f,0.05f,0.05f);
    glBegin(GL_QUADS);
    glVertex3f(-0.12f,1.42f+eyeAnger,-0.175f); glVertex3f(-0.04f,1.42f+eyeAnger,-0.175f);
    glVertex3f(-0.04f,1.50f,-0.175f);           glVertex3f(-0.12f,1.50f,-0.175f);
    glVertex3f( 0.04f,1.42f+eyeAnger,-0.175f); glVertex3f( 0.12f,1.42f+eyeAnger,-0.175f);
    glVertex3f( 0.12f,1.50f,-0.175f);           glVertex3f( 0.04f,1.50f,-0.175f);
    glEnd();

    /* Mouth */
    glColor3f(0.4f,0.1f,0.1f);
    glBegin(GL_QUADS);
    if (gBaldi.angry > 0) {
        glVertex3f(-0.10f,1.30f,-0.175f); glVertex3f(0.10f,1.30f,-0.175f);
        glVertex3f( 0.08f,1.35f,-0.175f); glVertex3f(-0.08f,1.35f,-0.175f);
    } else {
        glVertex3f(-0.10f,1.34f,-0.175f); glVertex3f(0.10f,1.34f,-0.175f);
        glVertex3f( 0.08f,1.30f,-0.175f); glVertex3f(-0.08f,1.30f,-0.175f);
    }
    glEnd();
    glPopMatrix();
}

void renderItems(void) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (!gItems[i].active) continue;
        float x = gItems[i].x, z = gItems[i].z;
        float rot = fmodf(gTime*40.0f + i*60.0f, 360.0f);

        glPushMatrix();
        glTranslatef(x, 0.25f, z);
        glRotatef(rot, 0,1,0);

        switch (gItems[i].type) {
        case 0: /* Energy Drink */
            drawBox(0,0,0,0.12f,0.25f,0.12f,(float[]){0.0f,0.9f,0.9f});
            glColor3f(1,1,1);
            glBegin(GL_QUADS);
            glVertex3f(-0.06f,0.18f,-0.061f); glVertex3f(0.06f,0.18f,-0.061f);
            glVertex3f(0.06f,0.25f,-0.061f);  glVertex3f(-0.06f,0.25f,-0.061f);
            glEnd(); break;
        case 1: /* Ruler */
            drawBox(0,0,0,0.04f,0.40f,0.04f,(float[]){0.8f,0.7f,0.1f}); break;
        case 2: /* B-Soda */
            drawBox(0,0,0,0.10f,0.30f,0.10f,(float[]){0.2f,0.3f,0.95f});
            drawBox(0,0.30f,0,0.06f,0.08f,0.06f,(float[]){0.5f,0.5f,0.5f}); break;
        case 3: /* Zesty Bar */
            drawBox(0,0,0,0.18f,0.06f,0.09f,(float[]){0.9f,0.5f,0.1f});
            glColor3f(1,0.9f,0);
            glBegin(GL_QUADS);
            glVertex3f(-0.09f,0.065f,-0.045f); glVertex3f(0.09f,0.065f,-0.045f);
            glVertex3f(0.09f,0.065f, 0.045f);  glVertex3f(-0.09f,0.065f,0.045f);
            glEnd(); break;
        case 4: /* Tape */
            drawBox(0,0,0,0.14f,0.10f,0.14f,(float[]){0.7f,0.7f,0.7f});
            drawBox(0,0,0,0.06f,0.11f,0.06f,(float[]){0.85f,0.85f,0.85f}); break;
        }
        glPopMatrix();
    }
}

void renderCloud(void) {
    if (!gCloud.active) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float alpha = 0.4f * (gCloud.timeLeft / 5.0f);
    glColor4f(0.3f, 0.5f, 1.0f, alpha);
    float r = gCloud.radius;
    for (int i = 0; i < 8; i++) {
        float a = i * 3.14159f / 4.0f;
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(gCloud.x, 1.0f, gCloud.z);
        for (int j = 0; j <= 16; j++) {
            float b = j * 2 * 3.14159f / 16.0f;
            glVertex3f(gCloud.x + r*cosf(b), 0.3f + r*sinf(a)*0.5f, gCloud.z + r*sinf(b));
        }
        glEnd();
    }
    glDisable(GL_BLEND);
}

/* ============================================================
 *  HUD
 * ============================================================ */
void drawHUD(void) {
    begin2D();

    float barW=180, barH=16, bx=10, by=gWindowH-30;
    float bgC[4]={0,0,0,0.6f};
    drawRect2D(bx, by, barW, barH, bgC);
    float sc[4]={COL_STAMINA[0],COL_STAMINA[1],COL_STAMINA[2],1.0f};
    drawRect2D(bx+1, by+1, barW*gPlayer.stamina-2, barH-2, sc);
    glColor3f(1,1,1);
    drawText2D(bx+barW+8, by+4, "STAMINA", GLUT_BITMAP_HELVETICA_12);

    char nbuf[64];
    snprintf(nbuf, sizeof(nbuf), "Buku: %d / %d", gPlayer.notebooksCollected, MAX_NOTEBOOKS);
    glColor3f(1,1,0.3f);
    drawText2D(10, gWindowH-55, nbuf, GLUT_BITMAP_HELVETICA_18);

    if (gPlayer.energyDrinks > 0) {
        char itbuf[32]; snprintf(itbuf, sizeof(itbuf), "[E] Energy: %d", gPlayer.energyDrinks);
        glColor3f(0,0.9f,0.9f); drawText2D(10, gWindowH-78, itbuf, GLUT_BITMAP_HELVETICA_12);
    }
    if (gPlayer.hasRuler) {
        glColor3f(0.9f,0.7f,0.1f); drawText2D(10, gWindowH-94, "[R] Ruler x1", GLUT_BITMAP_HELVETICA_12);
    }
    if (gPlayer.bsodaCount > 0) {
        char bbuf[32]; snprintf(bbuf, sizeof(bbuf), "[B] B-Soda: %d", gPlayer.bsodaCount);
        glColor3f(0.3f,0.5f,1.0f); drawText2D(10, gWindowH-110, bbuf, GLUT_BITMAP_HELVETICA_12);
    }

    /* Crosshair */
    float cx=gWindowW/2.0f, cy=gWindowH/2.0f;
    glColor3f(1,1,1);
    glBegin(GL_LINES);
    glVertex2f(cx-10,cy); glVertex2f(cx+10,cy);
    glVertex2f(cx,cy-10); glVertex2f(cx,cy+10);
    glEnd();

    if (gFlashTimer > 0) {
        float fa = clampf(gFlashTimer*0.5f, 0, 0.7f);
        float fc[4]={0.9f,0.05f,0.05f,fa};
        drawRect2D(0,0,gWindowW,gWindowH,fc);
    }

    if (gHudMsgTimer > 0) {
        float alpha = gHudMsgTimer > 1.0f ? 1.0f : gHudMsgTimer;
        glColor4f(1,1,1,alpha);
        float tx = (gWindowW - strlen(gHudMsg)*8.5f)/2.0f;
        drawText2D(tx, gWindowH/2.0f+60, gHudMsg, GLUT_BITMAP_HELVETICA_18);
    }

    if (gTime < 5.0f) {
        float a = gTime < 4.0f ? 1.0f : 5.0f-gTime;
        glColor4f(0.8f,0.8f,0.8f,a);
        drawText2D(gWindowW-320, 50, "WASD=Gerak  Mouse=Lihat  E=Ambil",  GLUT_BITMAP_HELVETICA_12);
        drawText2D(gWindowW-320, 32, "Shift=Sprint  B=B-Soda  ESC=Pause", GLUT_BITMAP_HELVETICA_12);
    }
    end2D();
}

/* ============================================================
 *  QUESTION POPUP
 * ============================================================ */
void drawQuestion(void) {
    begin2D();
    float pw=500, ph=200, px=(gWindowW-pw)/2.0f, py=(gWindowH-ph)/2.0f;
    float bg[4]={0.1f,0.1f,0.4f,0.92f};
    drawRect2D(px,py,pw,ph,bg);
    glColor3f(0.9f,0.9f,0.1f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(px,py); glVertex2f(px+pw,py); glVertex2f(px+pw,py+ph); glVertex2f(px,py+ph);
    glEnd();

    glColor3f(1,1,0.2f);
    drawText2D(px+20, py+ph-35, "=== SOAL MATEMATIKA ===", GLUT_BITMAP_HELVETICA_18);
    glColor3f(1,1,1);
    drawText2D(px+pw/2-80, py+ph-75, gQuestionText, GLUT_BITMAP_TIMES_ROMAN_24);

    char a1[32],a2[32],a3[32];
    snprintf(a1,sizeof(a1),"1) %d",gAnswerA);
    snprintf(a2,sizeof(a2),"2) %d",gAnswerB);
    snprintf(a3,sizeof(a3),"3) %d",gAnswerC);
    glColor3f(0.8f,1,0.8f);
    drawText2D(px+40,  py+90, a1, GLUT_BITMAP_HELVETICA_18);
    drawText2D(px+180, py+90, a2, GLUT_BITMAP_HELVETICA_18);
    drawText2D(px+320, py+90, a3, GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.7f,0.9f,1.0f);
    drawText2D(px+50, py+30, "Tekan 1, 2, atau 3 untuk menjawab", GLUT_BITMAP_HELVETICA_12);
    end2D();
}

/* ============================================================
 *  MENU SCREENS
 * ============================================================ */
void drawMenu(void) {
    begin2D();
    float bg[4]={0.05f,0.05f,0.10f,1.0f};
    drawRect2D(0,0,gWindowW,gWindowH,bg);

    glColor3f(0.9f,0.8f,0.1f);
    drawText2D(gWindowW/2-200, gWindowH-150, "BALDI'S BASICS 3D", GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.7f,0.7f,0.7f);
    drawText2D(gWindowW/2-180, gWindowH-185, "Kumpulkan Semua Buku - Hindari Baldi!", GLUT_BITMAP_HELVETICA_18);

    /* Baldi silhouette */
    float bx=gWindowW/2.0f, bby=gWindowH-340;
    float pulse=0.5f+0.5f*sinf(gTime*2.0f);
    drawRect2D(bx-30, bby+80, 60, 55,
               (float[]){0.95f,0.75f+pulse*0.1f,0.40f,1});
    drawRect2D(bx-35, bby+20, 70, 60, (float[]){0.9f,0.3f,0.1f,1});
    drawRect2D(bx-33, bby,    22, 22, (float[]){0.25f,0.25f,0.7f,1});
    drawRect2D(bx+10, bby,    22, 22, (float[]){0.25f,0.25f,0.7f,1});
    drawRect2D(bx-20, bby+115,12, 12, (float[]){0.05f,0.05f,0.05f,1});
    drawRect2D(bx+8,  bby+115,12, 12, (float[]){0.05f,0.05f,0.05f,1});

    const char *items[] = {"MULAI BERMAIN","CARA BERMAIN","KELUAR"};
    for (int i = 0; i < 3; i++) {
        float my = gWindowH/2.0f - 30 - i*50;
        if (i == gMenuSel) {
            float hl[4]={0.9f,0.8f,0.1f,0.25f};
            drawRect2D(gWindowW/2.0f-130, my-6, 260, 34, hl);
            glColor3f(1,1,0.2f);
        } else glColor3f(0.7f,0.7f,0.7f);
        int len = strlen(items[i]);
        drawText2D(gWindowW/2.0f - len*6.5f, my+4, items[i], GLUT_BITMAP_HELVETICA_18);
    }
    glColor3f(0.5f,0.5f,0.5f);
    drawText2D(gWindowW/2.0f-140, 30,
               "Panah Atas/Bawah=Pilih  Enter=Konfirmasi", GLUT_BITMAP_HELVETICA_12);
    end2D();
}

void drawInstructions(void) {
    begin2D();
    float bg[4]={0.05f,0.05f,0.15f,1.0f};
    drawRect2D(0,0,gWindowW,gWindowH,bg);
    glColor3f(0.9f,0.8f,0.1f);
    drawText2D(gWindowW/2.0f-110, gWindowH-80, "CARA BERMAIN", GLUT_BITMAP_TIMES_ROMAN_24);

    const char *lines[] = {
        "Tujuan: Kumpulkan semua 7 buku catatan (BIRU) lalu keluar",
        "melalui PINTU HIJAU sebelum Baldi menangkapmu!",
        "","KONTROL:",
        "  W / A / S / D  -  Bergerak",
        "  Mouse           -  Lihat ke sekitar",
        "  E atau Space    -  Ambil / Interaksi",
        "  Shift            -  Lari (menguras stamina)",
        "  B               -  Gunakan B-Soda (pukul Baldi)",
        "  ESC              -  Pause",
        "","ITEM:",
        "  CYAN   = Energy Drink (isi stamina)",
        "  KUNING = Ruler (buat Baldi melambat)",
        "  BIRU   = B-Soda (pukul Baldi dari jauh)",
        "  ORANYE = Zesty Bar (permanen+speed)",
        "  ABU    = Tape (bungkam Baldi sebentar)",
        "","TIPS:",
        "  - Setiap kali salah jawab matematika, Baldi MAKIN CEPAT!",
        "  - Berlari membuat stamina habis - jalan diam-diam lebih aman",
        "  - Perhatikan arah Baldi dan hindari jalan buntu!",
        "","Tekan ESC untuk kembali ke menu"
    };
    glColor3f(0.85f,0.85f,0.85f);
    int n = sizeof(lines)/sizeof(lines[0]);
    for (int i = 0; i < n; i++) {
        float y = gWindowH-130 - i*24;
        if (y < 20) break;
        drawText2D(60, y, lines[i], GLUT_BITMAP_HELVETICA_12);
    }
    end2D();
}

void drawWin(void) {
    begin2D();
    float bg[4]={0.0f,0.15f,0.05f,0.92f};
    drawRect2D(0,0,gWindowW,gWindowH,bg);
    glColor3f(0.2f,1,0.3f);
    drawText2D(gWindowW/2.0f-160, gWindowH/2.0f+60,
               "SELAMAT! KAMU MENANG!", GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(1,1,1);
    drawText2D(gWindowW/2.0f-200, gWindowH/2.0f+10,
               "Kamu berhasil kabur dari Baldi dan mengumpulkan semua buku!",
               GLUT_BITMAP_HELVETICA_12);
    char tbuf[64]; int sec=(int)gTime;
    snprintf(tbuf, sizeof(tbuf), "Waktu bermain: %d:%02d", sec/60, sec%60);
    glColor3f(0.9f,0.9f,0.4f);
    drawText2D(gWindowW/2.0f-80, gWindowH/2.0f-30, tbuf, GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.7f,0.7f,0.7f);
    drawText2D(gWindowW/2.0f-130, gWindowH/2.0f-80,
               "Tekan ENTER untuk main lagi  /  ESC untuk menu", GLUT_BITMAP_HELVETICA_12);
    end2D();
}

void drawLose(void) {
    begin2D();
    float bg[4]={0.15f,0.0f,0.0f,0.92f};
    drawRect2D(0,0,gWindowW,gWindowH,bg);
    float pulse=0.6f+0.4f*sinf(gTime*4.0f);
    glColor3f(pulse,0.1f,0.1f);
    drawText2D(gWindowW/2.0f-160, gWindowH/2.0f+60,
               "GAME OVER - KAMU TERTANGKAP!", GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(1,0.6f,0.6f);
    drawText2D(gWindowW/2.0f-210, gWindowH/2.0f+10,
               "Baldi menemukanmu dan membawa penggaris raksasanya!",
               GLUT_BITMAP_HELVETICA_12);
    char nbuf[64];
    snprintf(nbuf, sizeof(nbuf), "Buku terkumpul: %d / %d",
             gPlayer.notebooksCollected, MAX_NOTEBOOKS);
    glColor3f(0.9f,0.7f,0.3f);
    drawText2D(gWindowW/2.0f-80, gWindowH/2.0f-30, nbuf, GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.7f,0.7f,0.7f);
    drawText2D(gWindowW/2.0f-130, gWindowH/2.0f-80,
               "Tekan ENTER untuk main lagi  /  ESC untuk menu", GLUT_BITMAP_HELVETICA_12);
    end2D();
}

void drawPause(void) {
    begin2D();
    float bg[4]={0,0,0,0.65f};
    drawRect2D(0,0,gWindowW,gWindowH,bg);
    glColor3f(1,1,0.3f);
    drawText2D(gWindowW/2.0f-60, gWindowH/2.0f+30, "PAUSE", GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.8f,0.8f,0.8f);
    drawText2D(gWindowW/2.0f-130, gWindowH/2.0f-20,
               "ESC = Lanjut   Enter = Menu", GLUT_BITMAP_HELVETICA_18);
    end2D();
}