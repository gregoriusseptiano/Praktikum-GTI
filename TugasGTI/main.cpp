/*
 * ============================================================
 *  BALDI'S BASICS 3D - A FreeGLUT/OpenGL 3D Horror Game
 *  Gameplay: Collect all notebooks while avoiding Baldi!
 * ============================================================
 *
 * COMPILE:
 *   Linux/Mac:
 *     gcc -o baldis3d src/main.c -lGL -lGLU -lglut -lm
 *   Windows (MinGW):
 *     gcc -o baldis3d src/main.c -lfreeglut -lopengl32 -lglu32 -lm
 *
 * CONTROLS:
 *   W/A/S/D or Arrow Keys  - Move
 *   Mouse                  - Look around
 *   E or Space             - Interact / Pick up
 *   ESC                    - Pause / Quit
 * ============================================================
 */

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ============================================================
 *  CONSTANTS & CONFIG
 * ============================================================ */
#define WINDOW_W 1280
#define WINDOW_H 720
#define FOV 75.0f
#define MOVE_SPEED 4.0f
#define BALDI_SPEED_BASE 1.2f
#define MOUSE_SENS 0.15f

#define MAP_W 16
#define MAP_H 16
#define CELL_SIZE 2.0f
#define WALL_HEIGHT 2.5f

#define MAX_NOTEBOOKS 1
#define MAX_ITEMS 5
#define MAX_SOUNDS 8

/* ============================================================
 *  COLORS (RGB 0-1)
 * ============================================================ */
static const float COL_WALL_A[3] = {0.85f, 0.80f, 0.70f};
static const float COL_WALL_B[3] = {0.75f, 0.70f, 0.60f};
static const float COL_FLOOR[3] = {0.50f, 0.45f, 0.40f};
static const float COL_CEIL[3] = {0.90f, 0.88f, 0.82f};
static const float COL_NOTEBOOK[3] = {0.10f, 0.40f, 0.90f};
static const float COL_BALDI[3] = {0.95f, 0.75f, 0.40f};
// static const float COL_DOOR[3] = {0.60f, 0.35f, 0.10f};
// static const float COL_ITEM[3] = {0.90f, 0.80f, 0.10f};
// static const float COL_EXIT[3] = {0.10f, 0.90f, 0.20f};
static const float COL_STAMINA[3] = {0.20f, 0.80f, 0.20f};
// static const float COL_HUD_BG[4] = {0.0f, 0.0f, 0.0f, 0.5f};

/* ============================================================
 *  MAP (1=wall, 0=open, 2=door/exit)
 * ============================================================ */
static int map[MAP_H][MAP_W] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

/* ============================================================
 *  GAME STATE
 * ============================================================ */
typedef enum {
  STATE_MENU,
  STATE_PLAYING,
  STATE_PAUSED,
  STATE_WIN,
  STATE_LOSE,
  STATE_INSTRUCTIONS
} GameState;

typedef struct {
  float x, z;
  int collected;
  int active;
  float bobOffset;
  int questionAnswered; /* -1=none, 0-2=choices */
} Notebook;

typedef struct {
  float x, z;
  int type; /* 0=energy drink,1=ruler,2=bsoda,3=zesty bar,4=tape */
  int active;
} Item;

typedef struct {
  float x, z;
  float angle; /* facing angle degrees */
  float speed;
  int angry; /* 0=normal,1=angry,2=very angry */
  float angerTimer;
  int notebooksCollected; /* tracks player count to increase speed */
  float hearRadius;
} Baldi;

typedef struct {
  float x, z;
  float yaw;     /* horizontal angle */
  float pitch;   /* vertical angle */
  float stamina; /* 0-1 */
  int sprinting;
  int notebooksCollected;
  int energyDrinks;
  int hasRuler;
  int bsodaCount;
  float interactCooldown;
  float footstepTimer;
} Player;

typedef struct {
  int active;
  float x, z;
  float radius;
  float timeLeft;
} BsodaCloud; /* stuns Baldi */

/* ============================================================
 *  GLOBALS
 * ============================================================ */
static GameState gState = STATE_MENU;
static Player gPlayer;
static Baldi gBaldi;
static Notebook gNotebooks[MAX_NOTEBOOKS];
static Item gItems[MAX_ITEMS];
static BsodaCloud gCloud = {0};
static int gWindowW = WINDOW_W;
static int gWindowH = WINDOW_H;
static float gTime = 0.0f;
static int gMenuSel = 0;         /* 0=play,1=instructions,2=quit */
static float gFlashTimer = 0.0f; /* red flash when caught */
static int gMouseCaptured = 0;

/* Key states */
static int kW = 0, kA = 0, kS = 0, kD = 0, kShift = 0, kE = 0, kSpace = 0;
static int kUp = 0, kDown = 0, kLeft = 0, kRight = 0;

/* Question popup */
static int gShowQuestion = 0;
static int gQuestionIdx = -1;
static int gAnswerA, gAnswerB, gAnswerC, gCorrectAnswer;
static char gQuestionText[64];

/* HUD message */
static char gHudMsg[128] = "";
static float gHudMsgTimer = 0.0f;

/* ============================================================
 *  UTILITY
 * ============================================================ */
static float randf(float lo, float hi) {
  return lo + (hi - lo) * ((float)rand() / RAND_MAX);
}
static float dist2D(float ax, float az, float bx, float bz) {
  float dx = ax - bx, dz = az - bz;
  return sqrtf(dx * dx + dz * dz);
}
static float clampf(float v, float lo, float hi) {
  return v < lo ? lo : (v > hi ? hi : v);
}
static void setColor3fv(const float *c) { glColor3f(c[0], c[1], c[2]); }
static void setColor4fv(const float *c) { glColor4f(c[0], c[1], c[2], c[3]); }

/* ============================================================
 *  MAP HELPERS
 * ============================================================ */
static int mapAt(int col, int row) {
  if (col < 0 || col >= MAP_W || row < 0 || row >= MAP_H)
    return 1;
  return map[row][col];
}
static int mapSolid(int col, int row) {
  int v = mapAt(col, row);
  return v == 1;
}

/* World position -> map cell */
static void worldToCell(float wx, float wz, int *col, int *row) {
  *col = (int)(wx / CELL_SIZE);
  *row = (int)(wz / CELL_SIZE);
}

/* Check collision: try to move entity at (cx,cz) by (dx,dz), radius r */
static void moveWithCollision(float *cx, float *cz, float dx, float dz,
                              float r) {
  float nx = *cx + dx;
  float nz = *cz + dz;
  int col, row;

  /* X axis */
  worldToCell(nx, *cz, &col, &row);
  if (!mapSolid(col, row)) {
    /* also check corners */
    worldToCell(nx + r, *cz, &col, &row);
    int cA = mapSolid(col, row);
    worldToCell(nx - r, *cz, &col, &row);
    int cB = mapSolid(col, row);
    if (!cA && !cB)
      *cx = nx;
  }

  /* Z axis */
  worldToCell(*cx, nz, &col, &row);
  if (!mapSolid(col, row)) {
    worldToCell(*cx, nz + r, &col, &row);
    int cA = mapSolid(col, row);
    worldToCell(*cx, nz - r, &col, &row);
    int cB = mapSolid(col, row);
    if (!cA && !cB)
      *cz = nz;
  }
}

/* ============================================================
 *  GAME INIT
 * ============================================================ */
static void generateQuestion(int notebookIdx) {
  int a = (rand() % 9) + 1, b = (rand() % 9) + 1;
  int ops = rand() % 3;
  if (ops == 0) {
    snprintf(gQuestionText, sizeof(gQuestionText), "%d + %d = ?", a, b);
    gCorrectAnswer = a + b;
  } else if (ops == 1) {
    if (a < b) {
      int t = a;
      a = b;
      b = t;
    }
    snprintf(gQuestionText, sizeof(gQuestionText), "%d - %d = ?", a, b);
    gCorrectAnswer = a - b;
  } else {
    snprintf(gQuestionText, sizeof(gQuestionText), "%d x %d = ?", a, b);
    gCorrectAnswer = a * b;
  }
  /* generate 3 choices, one correct */
  int pos = rand() % 3;
  int choices[3];
  choices[pos] = gCorrectAnswer;
  for (int i = 0; i < 3; i++) {
    if (i == pos)
      continue;
    int wrong;
    do {
      wrong = gCorrectAnswer + (rand() % 7) - 3;
    } while (wrong == gCorrectAnswer || wrong < 0);
    choices[i] = wrong;
  }
  gAnswerA = choices[0];
  gAnswerB = choices[1];
  gAnswerC = choices[2];
  gQuestionIdx = notebookIdx;
  gShowQuestion = 1;
}

static void placeNotebooks(void) {
  /* Fixed spawn spots spread around the map */
  int sx[] = {8};
  int sz[] = {13};
  for (int i = 0; i < MAX_NOTEBOOKS; i++) {
    gNotebooks[i].x = sx[i] * CELL_SIZE + CELL_SIZE * 0.5f;
    gNotebooks[i].z = sz[i] * CELL_SIZE + CELL_SIZE * 0.5f;
    gNotebooks[i].collected = 0;
    gNotebooks[i].active = 1;
    gNotebooks[i].bobOffset = randf(0, 6.28f);
    gNotebooks[i].questionAnswered = -1;
  }
}

static void placeItems(void) {
  int sx[] = {4, 9, 2, 12, 7};
  int sz[] = {7, 9, 11, 5, 14};
  int types[] = {0, 1, 2, 3, 4};
  for (int i = 0; i < MAX_ITEMS; i++) {
    gItems[i].x = sx[i] * CELL_SIZE + CELL_SIZE * 0.5f;
    gItems[i].z = sz[i] * CELL_SIZE + CELL_SIZE * 0.5f;
    gItems[i].type = types[i];
    gItems[i].active = 1;
  }
}

static void initGame(void) {
  srand((unsigned)time(NULL));

  /* Player */
  gPlayer.x = 1.5f * CELL_SIZE;
  gPlayer.z = 1.5f * CELL_SIZE;
  gPlayer.yaw = 0.0f;
  gPlayer.pitch = 0.0f;
  gPlayer.stamina = 1.0f;
  gPlayer.sprinting = 0;
  gPlayer.notebooksCollected = 0;
  gPlayer.energyDrinks = 0;
  gPlayer.hasRuler = 0;
  gPlayer.bsodaCount = 0;
  gPlayer.interactCooldown = 0.0f;
  gPlayer.footstepTimer = 0.0f;

  /* Baldi */
  gBaldi.x = 13.5f * CELL_SIZE;
  gBaldi.z = 13.5f * CELL_SIZE;
  gBaldi.angle = 180.0f;
  gBaldi.speed = BALDI_SPEED_BASE;
  gBaldi.angry = 0;
  gBaldi.angerTimer = 0.0f;
  gBaldi.hearRadius = 8.0f;

  /* Objects */
  placeNotebooks();
  placeItems();

  /* Bsoda cloud */
  gCloud.active = 0;

  gFlashTimer = 0;
  gShowQuestion = 0;
  snprintf(gHudMsg, sizeof(gHudMsg), "Kumpulkan semua %d buku catatan!",
           MAX_NOTEBOOKS);
  gHudMsgTimer = 4.0f;
  gTime = 0.0f;
  gState = STATE_PLAYING;
}

/* ============================================================
 *  DRAWING HELPERS
 * ============================================================ */
static void drawText2D(float x, float y, const char *txt, void *font) {
  glRasterPos2f(x, y);
  for (int i = 0; txt[i]; i++)
    glutBitmapCharacter(font, txt[i]);
}

static void begin2D(void) {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, gWindowW, 0, gWindowH);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void end2D(void) {
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

/* Draw filled rectangle in 2D overlay */
static void drawRect2D(float x, float y, float w, float h, const float *col4) {
  setColor4fv(col4);
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + w, y);
  glVertex2f(x + w, y + h);
  glVertex2f(x, y + h);
  glEnd();
}

/* Draw a box: floor quad + 4 wall quads */
static void drawBox(float x, float y, float z, float sx, float sy, float sz,
                    const float *col) {
  setColor3fv(col);
  float x0 = x - sx / 2, x1 = x + sx / 2;
  float y0 = y, y1 = y + sy;
  float z0 = z - sz / 2, z1 = z + sz / 2;

  glBegin(GL_QUADS);
  /* Top */
  glVertex3f(x0, y1, z0);
  glVertex3f(x1, y1, z0);
  glVertex3f(x1, y1, z1);
  glVertex3f(x0, y1, z1);
  /* Bottom */
  glVertex3f(x0, y0, z1);
  glVertex3f(x1, y0, z1);
  glVertex3f(x1, y0, z0);
  glVertex3f(x0, y0, z0);
  /* Front */
  glVertex3f(x0, y0, z1);
  glVertex3f(x1, y0, z1);
  glVertex3f(x1, y1, z1);
  glVertex3f(x0, y1, z1);
  /* Back */
  glVertex3f(x0, y1, z0);
  glVertex3f(x1, y1, z0);
  glVertex3f(x1, y0, z0);
  glVertex3f(x0, y0, z0);
  /* Left */
  glVertex3f(x0, y0, z0);
  glVertex3f(x0, y0, z1);
  glVertex3f(x0, y1, z1);
  glVertex3f(x0, y1, z0);
  /* Right */
  glVertex3f(x1, y1, z0);
  glVertex3f(x1, y1, z1);
  glVertex3f(x1, y0, z1);
  glVertex3f(x1, y0, z0);
  glEnd();
}

/* ============================================================
 *  WORLD RENDERING
 * ============================================================ */
static void renderFloorAndCeiling(void) {
  glBegin(GL_QUADS);
  /* Floor */
  setColor3fv(COL_FLOOR);
  glVertex3f(0, 0, 0);
  glVertex3f(MAP_W * CELL_SIZE, 0, 0);
  glVertex3f(MAP_W * CELL_SIZE, 0, MAP_H * CELL_SIZE);
  glVertex3f(0, 0, MAP_H * CELL_SIZE);
  /* Ceiling */
  setColor3fv(COL_CEIL);
  glVertex3f(0, WALL_HEIGHT, MAP_H * CELL_SIZE);
  glVertex3f(MAP_W * CELL_SIZE, WALL_HEIGHT, MAP_H * CELL_SIZE);
  glVertex3f(MAP_W * CELL_SIZE, WALL_HEIGHT, 0);
  glVertex3f(0, WALL_HEIGHT, 0);
  glEnd();
}

static void renderWalls(void) {
  for (int row = 0; row < MAP_H; row++) {
    for (int col = 0; col < MAP_W; col++) {
      int v = mapAt(col, row);
      if (v == 0)
        continue;

      float wx = col * CELL_SIZE, wz = row * CELL_SIZE;
      /* Alternate color for texture feel */
      const float *wc = ((col + row) % 2 == 0) ? COL_WALL_A : COL_WALL_B;

      if (v == 2) {
        /* Exit door */
        float ec[3] = {0.10f, 0.85f, 0.20f};
        drawBox(wx + CELL_SIZE * 0.5f, 0, wz + CELL_SIZE * 0.5f, CELL_SIZE,
                WALL_HEIGHT, CELL_SIZE, ec);
        /* Sign */
        float sc[3] = {0.9f, 0.9f, 0.1f};
        drawBox(wx + CELL_SIZE * 0.5f, WALL_HEIGHT * 0.55f,
                wz + CELL_SIZE * 0.5f - 0.05f, 0.6f, 0.35f, 0.05f, sc);
      } else {
        drawBox(wx + CELL_SIZE * 0.5f, 0, wz + CELL_SIZE * 0.5f, CELL_SIZE,
                WALL_HEIGHT, CELL_SIZE, wc);
        /* Add stripe near top for variety */
        float stripe[3] = {wc[0] * 0.7f, wc[1] * 0.7f, wc[2] * 0.7f};
        drawBox(wx + CELL_SIZE * 0.5f, WALL_HEIGHT - 0.15f,
                wz + CELL_SIZE * 0.5f, CELL_SIZE + 0.01f, 0.15f,
                CELL_SIZE + 0.01f, stripe);
      }
    }
  }
}

/* Draw a notebook: a thin book standing upright */
static void renderNotebook(Notebook *nb) {
  if (!nb->active)
    return;
  float bob = sinf(gTime * 2.0f + nb->bobOffset) * 0.07f;
  float y = 0.4f + bob;
  float rot = fmodf(gTime * 30.0f, 360.0f);

  glPushMatrix();
  glTranslatef(nb->x, y, nb->z);
  glRotatef(rot, 0, 1, 0);
  /* Cover */
  setColor3fv(COL_NOTEBOOK);
  glBegin(GL_QUADS);
  glVertex3f(-0.12f, 0, -0.18f);
  glVertex3f(0.12f, 0, -0.18f);
  glVertex3f(0.12f, 0.30f, -0.18f);
  glVertex3f(-0.12f, 0.30f, -0.18f);
  glVertex3f(-0.12f, 0, 0.18f);
  glVertex3f(0.12f, 0, 0.18f);
  glVertex3f(0.12f, 0.30f, 0.18f);
  glVertex3f(-0.12f, 0.30f, 0.18f);
  glVertex3f(-0.12f, 0, -0.18f);
  glVertex3f(-0.12f, 0, 0.18f);
  glVertex3f(-0.12f, 0.30f, 0.18f);
  glVertex3f(-0.12f, 0.30f, -0.18f);
  glVertex3f(0.12f, 0.30f, -0.18f);
  glVertex3f(0.12f, 0.30f, 0.18f);
  glVertex3f(0.12f, 0, 0.18f);
  glVertex3f(0.12f, 0, -0.18f);
  glEnd();
  /* Pages (white) */
  glColor3f(0.95f, 0.95f, 0.92f);
  glBegin(GL_QUADS);
  glVertex3f(-0.10f, 0.01f, -0.17f);
  glVertex3f(0.10f, 0.01f, -0.17f);
  glVertex3f(0.10f, 0.28f, -0.17f);
  glVertex3f(-0.10f, 0.28f, -0.17f);
  glEnd();
  /* Lines on page */
  glColor3f(0.5f, 0.7f, 1.0f);
  glBegin(GL_LINES);
  for (float ly = 0.05f; ly < 0.26f; ly += 0.05f) {
    glVertex3f(-0.09f, ly, -0.169f);
    glVertex3f(0.09f, ly, -0.169f);
  }
  glEnd();
  glPopMatrix();
}

/* Baldi character: simple blocky humanoid */
static void renderBaldi(void) {
  float bx = gBaldi.x, bz = gBaldi.z;

  /* Angry color blend */
  float anger = (gBaldi.angry == 2) ? 1.0f : (gBaldi.angry == 1) ? 0.5f : 0.0f;
  float headR = COL_BALDI[0] * (1 - anger) + 0.95f * anger;
  float headG = COL_BALDI[1] * (1 - anger) + 0.20f * anger;
  float headB = COL_BALDI[2] * (1 - anger) + 0.10f * anger;

  /* Body sway */
  float sway = sinf(gTime * 8.0f) * 0.03f;

  glPushMatrix();
  glTranslatef(bx, 0, bz);
  glRotatef(-gBaldi.angle, 0, 1, 0);

  /* Legs */
  glColor3f(0.25f, 0.25f, 0.70f);
  float legSwing = sinf(gTime * 8.0f) * 0.15f;
  /* Left leg */
  glPushMatrix();
  glTranslatef(-0.12f, 0, sway);
  glRotatef(legSwing * 30, 1, 0, 0);
  drawBox(0, 0, 0, 0.18f, 0.55f, 0.18f, (float[]){0.25f, 0.25f, 0.70f});
  glPopMatrix();
  /* Right leg */
  glPushMatrix();
  glTranslatef(0.12f, 0, -sway);
  glRotatef(-legSwing * 30, 1, 0, 0);
  drawBox(0, 0, 0, 0.18f, 0.55f, 0.18f, (float[]){0.25f, 0.25f, 0.70f});
  glPopMatrix();

  /* Torso */
  drawBox(0, 0.55f, 0, 0.45f, 0.60f, 0.30f, (float[]){0.90f, 0.30f, 0.10f});

  /* Ruler (arm) */
  glPushMatrix();
  glTranslatef(0.30f, 0.90f, 0);
  glRotatef(-30 + sinf(gTime * 6) * 20, 1, 0, 0);
  drawBox(0, 0, 0, 0.06f, 0.55f, 0.06f, (float[]){0.85f, 0.65f, 0.10f});
  glPopMatrix();

  /* Left arm */
  glPushMatrix();
  glTranslatef(-0.30f, 0.90f, 0);
  glRotatef(20 + cosf(gTime * 6) * 15, 1, 0, 0);
  drawBox(0, 0, 0, 0.12f, 0.45f, 0.12f, (float[]){0.95f, 0.75f, 0.40f});
  glPopMatrix();

  /* Head */
  float headCol[3] = {headR, headG, headB};
  drawBox(0, 1.25f, 0, 0.40f, 0.40f, 0.35f, headCol);

  /* Eyes */
  glColor3f(0.05f, 0.05f, 0.05f);
  float eyeAnger = anger * 0.05f;
  glBegin(GL_QUADS);
  /* Left eye */
  glVertex3f(-0.12f, 1.42f + eyeAnger, -0.175f);
  glVertex3f(-0.04f, 1.42f + eyeAnger, -0.175f);
  glVertex3f(-0.04f, 1.50f, -0.175f);
  glVertex3f(-0.12f, 1.50f, -0.175f);
  /* Right eye */
  glVertex3f(0.04f, 1.42f + eyeAnger, -0.175f);
  glVertex3f(0.12f, 1.42f + eyeAnger, -0.175f);
  glVertex3f(0.12f, 1.50f, -0.175f);
  glVertex3f(0.04f, 1.50f, -0.175f);
  glEnd();

  /* Smile (or frown) */
  glColor3f(0.4f, 0.1f, 0.1f);
  glBegin(GL_QUADS);
  if (gBaldi.angry > 0) {
    /* Frown */
    glVertex3f(-0.10f, 1.30f, -0.175f);
    glVertex3f(0.10f, 1.30f, -0.175f);
    glVertex3f(0.08f, 1.35f, -0.175f);
    glVertex3f(-0.08f, 1.35f, -0.175f);
  } else {
    /* Smile */
    glVertex3f(-0.10f, 1.34f, -0.175f);
    glVertex3f(0.10f, 1.34f, -0.175f);
    glVertex3f(0.08f, 1.30f, -0.175f);
    glVertex3f(-0.08f, 1.30f, -0.175f);
  }
  glEnd();

  glPopMatrix();
}

/* Items on the floor */
static void renderItems(void) {
  for (int i = 0; i < MAX_ITEMS; i++) {
    if (!gItems[i].active)
      continue;
    float x = gItems[i].x, z = gItems[i].z;
    float rot = fmodf(gTime * 40.0f + i * 60.0f, 360.0f);

    glPushMatrix();
    glTranslatef(x, 0.25f, z);
    glRotatef(rot, 0, 1, 0);

    switch (gItems[i].type) {
    case 0: /* Energy Drink - cyan can */
      drawBox(0, 0, 0, 0.12f, 0.25f, 0.12f, (float[]){0.0f, 0.9f, 0.9f});
      glColor3f(1, 1, 1);
      glBegin(GL_QUADS);
      glVertex3f(-0.06f, 0.18f, -0.061f);
      glVertex3f(0.06f, 0.18f, -0.061f);
      glVertex3f(0.06f, 0.25f, -0.061f);
      glVertex3f(-0.06f, 0.25f, -0.061f);
      glEnd();
      break;
    case 1: /* Ruler */
      drawBox(0, 0, 0, 0.04f, 0.40f, 0.04f, (float[]){0.8f, 0.7f, 0.1f});
      break;
    case 2: /* B-Soda - blue bottle */
      drawBox(0, 0, 0, 0.10f, 0.30f, 0.10f, (float[]){0.2f, 0.3f, 0.95f});
      drawBox(0, 0.30f, 0, 0.06f, 0.08f, 0.06f, (float[]){0.5f, 0.5f, 0.5f});
      break;
    case 3: /* Zesty Bar */
      drawBox(0, 0, 0, 0.18f, 0.06f, 0.09f, (float[]){0.9f, 0.5f, 0.1f});
      glColor3f(1, 0.9f, 0);
      glBegin(GL_QUADS);
      glVertex3f(-0.09f, 0.065f, -0.045f);
      glVertex3f(0.09f, 0.065f, -0.045f);
      glVertex3f(0.09f, 0.065f, 0.045f);
      glVertex3f(-0.09f, 0.065f, 0.045f);
      glEnd();
      break;
    case 4: /* Tape - grey roll */
      drawBox(0, 0, 0, 0.14f, 0.10f, 0.14f, (float[]){0.7f, 0.7f, 0.7f});
      drawBox(0, 0, 0, 0.06f, 0.11f, 0.06f, (float[]){0.85f, 0.85f, 0.85f});
      break;
    }
    glPopMatrix();
  }
}

/* BSOda stun cloud */
static void renderCloud(void) {
  if (!gCloud.active)
    return;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  float alpha = 0.4f * (gCloud.timeLeft / 5.0f);
  glColor4f(0.3f, 0.5f, 1.0f, alpha);
  /* Draw simple sphere-like shape */
  for (int i = 0; i < 8; i++) {
    float a = i * 3.14159f / 4.0f;
    float r = gCloud.radius;
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(gCloud.x, 1.0f, gCloud.z);
    for (int j = 0; j <= 16; j++) {
      float b = j * 2 * 3.14159f / 16.0f;
      glVertex3f(gCloud.x + r * cosf(b), 0.3f + r * sinf(a) * 0.5f,
                 gCloud.z + r * sinf(b));
    }
    glEnd();
  }
  glDisable(GL_BLEND);
}

/* ============================================================
 *  CAMERA SETUP
 * ============================================================ */
static void setupCamera(void) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(FOV, (float)gWindowW / gWindowH, 0.05f, 100.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  float yaw_r = gPlayer.yaw * 3.14159f / 180.0f;
  float pitch_r = gPlayer.pitch * 3.14159f / 180.0f;

  float eyeX = gPlayer.x;
  float eyeY = 1.10f; /* eye height */
  float eyeZ = gPlayer.z;

  float lx = eyeX + cosf(yaw_r) * cosf(pitch_r);
  float ly = eyeY + sinf(pitch_r);
  float lz = eyeZ + sinf(yaw_r) * cosf(pitch_r);

  gluLookAt(eyeX, eyeY, eyeZ, lx, ly, lz, 0, 1, 0);
}

/* ============================================================
 *  HUD DRAWING
 * ============================================================ */
static void drawHUD(void) {
  begin2D();

  /* Stamina bar */
  float barW = 180, barH = 16;
  float bx = 10, by = gWindowH - 30;
  float bgC[4] = {0, 0, 0, 0.6f};
  drawRect2D(bx, by, barW, barH, bgC);
  float sc[4] = {COL_STAMINA[0], COL_STAMINA[1], COL_STAMINA[2], 1.0f};
  drawRect2D(bx + 1, by + 1, barW * gPlayer.stamina - 2, barH - 2, sc);

  glColor3f(1, 1, 1);
  drawText2D(bx + barW + 8, by + 4, "STAMINA", GLUT_BITMAP_HELVETICA_12);

  /* Notebook counter */
  char nbuf[64];
  snprintf(nbuf, sizeof(nbuf), "Buku: %d / %d", gPlayer.notebooksCollected,
           MAX_NOTEBOOKS);
  glColor3f(1, 1, 0.3f);
  drawText2D(10, gWindowH - 55, nbuf, GLUT_BITMAP_HELVETICA_18);

  /* Items in inventory */
  if (gPlayer.energyDrinks > 0) {
    char itbuf[32];
    snprintf(itbuf, sizeof(itbuf), "[E] Energy: %d", gPlayer.energyDrinks);
    glColor3f(0, 0.9f, 0.9f);
    drawText2D(10, gWindowH - 78, itbuf, GLUT_BITMAP_HELVETICA_12);
  }
  if (gPlayer.hasRuler) {
    glColor3f(0.9f, 0.7f, 0.1f);
    drawText2D(10, gWindowH - 94, "[R] Ruler x1", GLUT_BITMAP_HELVETICA_12);
  }
  if (gPlayer.bsodaCount > 0) {
    char bbuf[32];
    snprintf(bbuf, sizeof(bbuf), "[B] B-Soda: %d", gPlayer.bsodaCount);
    glColor3f(0.3f, 0.5f, 1.0f);
    drawText2D(10, gWindowH - 110, bbuf, GLUT_BITMAP_HELVETICA_12);
  }

  /* Crosshair */
  float cx = gWindowW / 2.0f, cy = gWindowH / 2.0f;
  glColor3f(1, 1, 1);
  glBegin(GL_LINES);
  glVertex2f(cx - 10, cy);
  glVertex2f(cx + 10, cy);
  glVertex2f(cx, cy - 10);
  glVertex2f(cx, cy + 10);
  glEnd();

  /* Flash overlay when caught */
  if (gFlashTimer > 0) {
    float fa = gFlashTimer * 0.5f;
    if (fa > 0.7f)
      fa = 0.7f;
    float fc[4] = {0.9f, 0.05f, 0.05f, fa};
    drawRect2D(0, 0, gWindowW, gWindowH, fc);
  }

  /* HUD message */
  if (gHudMsgTimer > 0) {
    float alpha = gHudMsgTimer > 1.0f ? 1.0f : gHudMsgTimer;
    glColor4f(1, 1, 1, alpha);
    /* Center text approximately */
    int len = strlen(gHudMsg);
    float tx = (gWindowW - len * 8.5f) / 2.0f;
    drawText2D(tx, gWindowH / 2.0f + 60, gHudMsg, GLUT_BITMAP_HELVETICA_18);
  }

  /* Controls reminder at start */
  if (gTime < 5.0f) {
    float a = gTime < 4.0f ? 1.0f : 5.0f - gTime;
    glColor4f(0.8f, 0.8f, 0.8f, a);
    drawText2D(gWindowW - 320, 50, "WASD=Gerak  Mouse=Lihat  E=Ambil",
               GLUT_BITMAP_HELVETICA_12);
    drawText2D(gWindowW - 320, 32, "Shift=Sprint  B=B-Soda  ESC=Pause",
               GLUT_BITMAP_HELVETICA_12);
  }

  end2D();
}

/* ============================================================
 *  QUESTION POPUP
 * ============================================================ */
static void drawQuestion(void) {
  begin2D();

  float pw = 500, ph = 200;
  float px = (gWindowW - pw) / 2.0f, py = (gWindowH - ph) / 2.0f;
  float bg[4] = {0.1f, 0.1f, 0.4f, 0.92f};
  drawRect2D(px, py, pw, ph, bg);
  /* Border */
  glColor3f(0.9f, 0.9f, 0.1f);
  glBegin(GL_LINE_LOOP);
  glVertex2f(px, py);
  glVertex2f(px + pw, py);
  glVertex2f(px + pw, py + ph);
  glVertex2f(px, py + ph);
  glEnd();

  glColor3f(1, 1, 0.2f);
  drawText2D(px + 20, py + ph - 35,
             "=== SOAL MATEMATIKA ===", GLUT_BITMAP_HELVETICA_18);

  glColor3f(1, 1, 1);
  drawText2D(px + pw / 2 - 80, py + ph - 75, gQuestionText,
             GLUT_BITMAP_TIMES_ROMAN_24);

  char a1[32], a2[32], a3[32];
  snprintf(a1, sizeof(a1), "1) %d", gAnswerA);
  snprintf(a2, sizeof(a2), "2) %d", gAnswerB);
  snprintf(a3, sizeof(a3), "3) %d", gAnswerC);

  glColor3f(0.8f, 1, 0.8f);
  drawText2D(px + 40, py + 90, a1, GLUT_BITMAP_HELVETICA_18);
  drawText2D(px + 180, py + 90, a2, GLUT_BITMAP_HELVETICA_18);
  drawText2D(px + 320, py + 90, a3, GLUT_BITMAP_HELVETICA_18);

  glColor3f(0.7f, 0.9f, 1.0f);
  drawText2D(px + 50, py + 30, "Tekan 1, 2, atau 3 untuk menjawab",
             GLUT_BITMAP_HELVETICA_12);

  end2D();
}

/* ============================================================
 *  MENU SCREENS
 * ============================================================ */
static void drawMenu(void) {
  begin2D();

  /* Dark background */
  float bg[4] = {0.05f, 0.05f, 0.10f, 1.0f};
  drawRect2D(0, 0, gWindowW, gWindowH, bg);

  /* Title */
  glColor3f(0.9f, 0.8f, 0.1f);
  drawText2D(gWindowW / 2 - 200, gWindowH - 150, "BALDI'S BASICS 3D",
             GLUT_BITMAP_TIMES_ROMAN_24);
  glColor3f(0.7f, 0.7f, 0.7f);
  drawText2D(gWindowW / 2 - 180, gWindowH - 185,
             "Kumpulkan Semua Buku - Hindari Baldi!", GLUT_BITMAP_HELVETICA_18);

  /* Draw simple Baldi silhouette (2D boxes) */
  float bx = gWindowW / 2.0f, bby = gWindowH - 340;
  float pulse = 0.5f + 0.5f * sinf(gTime * 2.0f);
  glColor3f(0.95f, 0.75f, 0.40f);
  /* head */
  drawRect2D(bx - 30, bby + 80, 60, 55,
             (float[]){0.95f, 0.75f + pulse * 0.1f, 0.40f, 1});
  /* body */
  drawRect2D(bx - 35, bby + 20, 70, 60, (float[]){0.9f, 0.3f, 0.1f, 1});
  /* legs */
  drawRect2D(bx - 33, bby, 22, 22, (float[]){0.25f, 0.25f, 0.7f, 1});
  drawRect2D(bx + 10, bby, 22, 22, (float[]){0.25f, 0.25f, 0.7f, 1});
  /* eyes */
  glColor3f(0.05f, 0.05f, 0.05f);
  drawRect2D(bx - 20, bby + 115, 12, 12, (float[]){0.05f, 0.05f, 0.05f, 1});
  drawRect2D(bx + 8, bby + 115, 12, 12, (float[]){0.05f, 0.05f, 0.05f, 1});

  /* Menu items */
  const char *items[] = {"MULAI BERMAIN", "CARA BERMAIN", "KELUAR"};
  for (int i = 0; i < 3; i++) {
    float my = gWindowH / 2.0f - 30 - i * 50;
    if (i == gMenuSel) {
      float hl[4] = {0.9f, 0.8f, 0.1f, 0.25f};
      drawRect2D(gWindowW / 2.0f - 130, my - 6, 260, 34, hl);
      glColor3f(1, 1, 0.2f);
    } else {
      glColor3f(0.7f, 0.7f, 0.7f);
    }
    int len = strlen(items[i]);
    drawText2D(gWindowW / 2.0f - len * 6.5f, my + 4, items[i],
               GLUT_BITMAP_HELVETICA_18);
  }
  glColor3f(0.5f, 0.5f, 0.5f);
  drawText2D(gWindowW / 2.0f - 140, 30,
             "Panah Atas/Bawah=Pilih  Enter=Konfirmasi",
             GLUT_BITMAP_HELVETICA_12);

  end2D();
}

static void drawInstructions(void) {
  begin2D();
  float bg[4] = {0.05f, 0.05f, 0.15f, 1.0f};
  drawRect2D(0, 0, gWindowW, gWindowH, bg);

  glColor3f(0.9f, 0.8f, 0.1f);
  drawText2D(gWindowW / 2.0f - 110, gWindowH - 80, "CARA BERMAIN",
             GLUT_BITMAP_TIMES_ROMAN_24);

  const char *lines[] = {
      "Tujuan: Kumpulkan semua 7 buku catatan (BIRU) lalu keluar",
      "melalui PINTU HIJAU sebelum Baldi menangkapmu!",
      "",
      "KONTROL:",
      "  W / A / S / D  -  Bergerak",
      "  Mouse           -  Lihat ke sekitar",
      "  E atau Space    -  Ambil / Interaksi",
      "  Shift            -  Lari (menguras stamina)",
      "  B               -  Gunakan B-Soda (pukul Baldi)",
      "  ESC              -  Pause",
      "",
      "ITEM:",
      "  CYAN   = Energy Drink (isi stamina)",
      "  KUNING = Ruler (buat Baldi melambat)",
      "  BIRU   = B-Soda (pukul Baldi dari jauh)",
      "  ORANYE = Zesty Bar (permanen+speed)",
      "  ABU    = Tape (bungkam Baldi sebentar)",
      "",
      "TIPS:",
      "  - Setiap kali salah jawab matematika, Baldi MAKIN CEPAT!",
      "  - Berlari membuat stamina habis - jalan diam-diam lebih aman",
      "  - Perhatikan arah Baldi dan hindari jalan buntu!",
      "",
      "Tekan ESC untuk kembali ke menu"};

  glColor3f(0.85f, 0.85f, 0.85f);
  int n = sizeof(lines) / sizeof(lines[0]);
  for (int i = 0; i < n; i++) {
    float y = gWindowH - 130 - i * 24;
    if (y < 20)
      break;
    drawText2D(60, y, lines[i], GLUT_BITMAP_HELVETICA_12);
  }
  end2D();
}

static void drawWin(void) {
  begin2D();
  float bg[4] = {0.0f, 0.15f, 0.05f, 0.92f};
  drawRect2D(0, 0, gWindowW, gWindowH, bg);

  glColor3f(0.2f, 1, 0.3f);
  drawText2D(gWindowW / 2.0f - 160, gWindowH / 2.0f + 60,
             "SELAMAT! KAMU MENANG!", GLUT_BITMAP_TIMES_ROMAN_24);
  glColor3f(1, 1, 1);
  drawText2D(gWindowW / 2.0f - 200, gWindowH / 2.0f + 10,
             "Kamu berhasil kabur dari Baldi dan mengumpulkan semua buku!",
             GLUT_BITMAP_HELVETICA_12);
  char tbuf[64];
  int sec = (int)gTime;
  snprintf(tbuf, sizeof(tbuf), "Waktu bermain: %d:%02d", sec / 60, sec % 60);
  glColor3f(0.9f, 0.9f, 0.4f);
  drawText2D(gWindowW / 2.0f - 80, gWindowH / 2.0f - 30, tbuf,
             GLUT_BITMAP_HELVETICA_18);
  glColor3f(0.7f, 0.7f, 0.7f);
  drawText2D(gWindowW / 2.0f - 130, gWindowH / 2.0f - 80,
             "Tekan ENTER untuk main lagi  /  ESC untuk menu",
             GLUT_BITMAP_HELVETICA_12);
  end2D();
}

static void drawLose(void) {
  begin2D();
  float bg[4] = {0.15f, 0.0f, 0.0f, 0.92f};
  drawRect2D(0, 0, gWindowW, gWindowH, bg);

  float pulse = 0.6f + 0.4f * sinf(gTime * 4.0f);
  glColor3f(pulse, 0.1f, 0.1f);
  drawText2D(gWindowW / 2.0f - 160, gWindowH / 2.0f + 60,
             "GAME OVER - KAMU TERTANGKAP!", GLUT_BITMAP_TIMES_ROMAN_24);
  glColor3f(1, 0.6f, 0.6f);
  drawText2D(gWindowW / 2.0f - 210, gWindowH / 2.0f + 10,
             "Baldi menemukanmu dan membawa penggaris raksasanya!",
             GLUT_BITMAP_HELVETICA_12);
  char nbuf[64];
  snprintf(nbuf, sizeof(nbuf), "Buku terkumpul: %d / %d",
           gPlayer.notebooksCollected, MAX_NOTEBOOKS);
  glColor3f(0.9f, 0.7f, 0.3f);
  drawText2D(gWindowW / 2.0f - 80, gWindowH / 2.0f - 30, nbuf,
             GLUT_BITMAP_HELVETICA_18);
  glColor3f(0.7f, 0.7f, 0.7f);
  drawText2D(gWindowW / 2.0f - 130, gWindowH / 2.0f - 80,
             "Tekan ENTER untuk main lagi  /  ESC untuk menu",
             GLUT_BITMAP_HELVETICA_12);
  end2D();
}

static void drawPause(void) {
  begin2D();
  float bg[4] = {0, 0, 0, 0.65f};
  drawRect2D(0, 0, gWindowW, gWindowH, bg);
  glColor3f(1, 1, 0.3f);
  drawText2D(gWindowW / 2.0f - 60, gWindowH / 2.0f + 30, "PAUSE",
             GLUT_BITMAP_TIMES_ROMAN_24);
  glColor3f(0.8f, 0.8f, 0.8f);
  drawText2D(gWindowW / 2.0f - 130, gWindowH / 2.0f - 20,
             "ESC = Lanjut   Enter = Menu", GLUT_BITMAP_HELVETICA_18);
  end2D();
}

/* ============================================================
 *  AI - BALDI PATHFINDING (simple waypoint / line of sight)
 * ============================================================ */
static void updateBaldi(float dt) {
  if (gState != STATE_PLAYING)
    return;

  /* If stunned by cloud */
  if (gCloud.active) {
    float d = dist2D(gBaldi.x, gBaldi.z, gCloud.x, gCloud.z);
    if (d < gCloud.radius) {
      /* Baldi is stunned */
      return;
    }
  }

  /* Move Baldi toward player using simple steering */
  float dx = gPlayer.x - gBaldi.x;
  float dz = gPlayer.z - gBaldi.z;
  float dist = sqrtf(dx * dx + dz * dz);

  if (dist < 0.01f)
    return;

  /* Normalize */
  float nx = dx / dist, nz = dz / dist;

  /* Can Baldi "hear" the player? */
  int canHear = (dist < gBaldi.hearRadius);
  /* Sprinting makes player louder */
  if (gPlayer.sprinting)
    canHear = (dist < gBaldi.hearRadius * 1.5f);

  if (!canHear && gBaldi.angry == 0)
    return;

  /* Speed increases with each notebook */
  float sp = gBaldi.speed;
  if (gBaldi.angry == 1)
    sp *= 1.3f;
  if (gBaldi.angry == 2)
    sp *= 1.7f;

  float mx = nx * sp * dt;
  float mz = nz * sp * dt;
  moveWithCollision(&gBaldi.x, &gBaldi.z, mx, mz, 0.3f);

  /* Update facing angle */
  gBaldi.angle = atan2f(nz, nx) * 180.0f / 3.14159f + 90.0f;

  /* Catch player? */
  float catchDist = dist2D(gPlayer.x, gPlayer.z, gBaldi.x, gBaldi.z);
  if (catchDist < 0.55f) {
    gState = STATE_LOSE;
    if (gMouseCaptured) {
      glutSetCursor(GLUT_CURSOR_INHERIT);
      gMouseCaptured = 0;
    }
  }

  /* Anger timer decay */
  if (gBaldi.angerTimer > 0) {
    gBaldi.angerTimer -= dt;
    if (gBaldi.angerTimer <= 0) {
      gBaldi.angry = 0;
    }
  }
}

/* ============================================================
 *  PLAYER UPDATE
 * ============================================================ */
static void updatePlayer(float dt) {
  if (gState != STATE_PLAYING)
    return;

  /* Sprinting */
  gPlayer.sprinting = (kShift && gPlayer.stamina > 0.0f) ? 1 : 0;
  float speed = MOVE_SPEED * dt * (gPlayer.sprinting ? 1.6f : 1.0f);

  /* Stamina */
  if (gPlayer.sprinting) {
    gPlayer.stamina -= dt * 0.35f;
    if (gPlayer.stamina < 0)
      gPlayer.stamina = 0;
  } else {
    gPlayer.stamina += dt * 0.25f;
    if (gPlayer.stamina > 1)
      gPlayer.stamina = 1;
  }

  float yaw_r = gPlayer.yaw * 3.14159f / 180.0f;
  float fx = cosf(yaw_r), fz = sinf(yaw_r); /* forward */
  float rx = -fz, rz = fx;                  /* right   */

  /* Arrow keys for camera */
  if (kLeft)
    gPlayer.yaw -= 120.0f * dt;
  if (kRight)
    gPlayer.yaw += 120.0f * dt;
  if (kUp)
    gPlayer.pitch += 120.0f * dt;
  if (kDown)
    gPlayer.pitch -= 120.0f * dt;
  gPlayer.pitch = clampf(gPlayer.pitch, -70, 70);

  float dx = 0, dz = 0;
  if (kW) {
    dx += fx;
    dz += fz;
  }
  if (kS) {
    dx -= fx;
    dz -= fz;
  }
  if (kA) {
    dx -= rx;
    dz -= rz;
  }
  if (kD) {
    dx += rx;
    dz += rz;
  }

  float len = sqrtf(dx * dx + dz * dz);
  if (len > 0) {
    dx /= len;
    dz /= len;
  }
  dx *= speed;
  dz *= speed;

  moveWithCollision(&gPlayer.x, &gPlayer.z, dx, dz, 0.25f);

  /* Interact cooldown */
  if (gPlayer.interactCooldown > 0)
    gPlayer.interactCooldown -= dt;

  /* Check notebook pickup */
  if ((kE || kSpace) && !gShowQuestion && gPlayer.interactCooldown <= 0) {
    for (int i = 0; i < MAX_NOTEBOOKS; i++) {
      if (!gNotebooks[i].active)
        continue;
      float d = dist2D(gPlayer.x, gPlayer.z, gNotebooks[i].x, gNotebooks[i].z);
      if (d < 0.9f) {
        /* Open question */
        generateQuestion(i);
        gPlayer.interactCooldown = 0.5f;
        break;
      }
    }
    /* Check item pickup */
    for (int i = 0; i < MAX_ITEMS; i++) {
      if (!gItems[i].active)
        continue;
      float d = dist2D(gPlayer.x, gPlayer.z, gItems[i].x, gItems[i].z);
      if (d < 0.9f) {
        gItems[i].active = 0;
        switch (gItems[i].type) {
        case 0:
          gPlayer.energyDrinks++;
          snprintf(gHudMsg, sizeof(gHudMsg),
                   "Dapat Energy Drink! Tekan E untuk pakai");
          break;
        case 1:
          gPlayer.hasRuler = 1;
          snprintf(gHudMsg, sizeof(gHudMsg),
                   "Dapat Ruler! Tekan R untuk pakai");
          break;
        case 2:
          gPlayer.bsodaCount++;
          snprintf(gHudMsg, sizeof(gHudMsg),
                   "Dapat B-Soda! Tekan B untuk lempar ke Baldi");
          break;
        case 3:
          speed += 0.01f; /* permanent slight boost */
          snprintf(gHudMsg, sizeof(gHudMsg), "Zesty Bar! Speed naik permanen!");
          break;
        case 4:
          snprintf(gHudMsg, sizeof(gHudMsg),
                   "Principal Tape! Baldi diam sebentar!");
          gBaldi.angerTimer = 0;
          gBaldi.angry = 0;
          break;
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

  /* Use B-Soda */
  static int prevB = 0;
  /* extern int kB; */
  if (!prevB && gPlayer.bsodaCount > 0) {
    /* kB checked in keyboard callback */
  }
  prevB = 0;

  /* Check exit */
  int col, row;
  worldToCell(gPlayer.x, gPlayer.z, &col, &row);
  if (mapAt(col, row) == 2 && gPlayer.notebooksCollected == MAX_NOTEBOOKS) {
    gState = STATE_WIN;
    if (gMouseCaptured) {
      glutSetCursor(GLUT_CURSOR_INHERIT);
      gMouseCaptured = 0;
    }
  } else if (mapAt(col, row) == 2 &&
             gPlayer.notebooksCollected < MAX_NOTEBOOKS) {
    snprintf(gHudMsg, sizeof(gHudMsg), "Kumpulkan semua buku dulu! (%d/%d)",
             gPlayer.notebooksCollected, MAX_NOTEBOOKS);
    gHudMsgTimer = 2.0f;
  }

  /* Update flash */
  if (gFlashTimer > 0)
    gFlashTimer -= dt;
  if (gHudMsgTimer > 0)
    gHudMsgTimer -= dt;
}

/* ============================================================
 *  ANSWER QUESTION
 * ============================================================ */
static void answerQuestion(int choice) {
  /* int *answers = {0}; */
  int chosen;
  if (choice == 1)
    chosen = gAnswerA;
  else if (choice == 2)
    chosen = gAnswerB;
  else
    chosen = gAnswerC;

  if (chosen == gCorrectAnswer) {
    /* Correct! */
    gNotebooks[gQuestionIdx].active = 0;
    gNotebooks[gQuestionIdx].collected = 1;
    gPlayer.notebooksCollected++;
    snprintf(gHudMsg, sizeof(gHudMsg), "Benar! Buku %d/%d terkumpul!",
             gPlayer.notebooksCollected, MAX_NOTEBOOKS);
    gHudMsgTimer = 3.0f;
    /* Baldi hears this and comes */
    gBaldi.hearRadius = 20.0f; /* permanent increase */
    gBaldi.speed = BALDI_SPEED_BASE + gPlayer.notebooksCollected * 0.25f;
  } else {
    /* Wrong! Baldi gets angry */
    snprintf(gHudMsg, sizeof(gHudMsg), "SALAH! Baldi makin cepat!");
    gHudMsgTimer = 3.0f;
    gFlashTimer = 1.0f;
    gBaldi.angry++;
    if (gBaldi.angry > 2)
      gBaldi.angry = 2;
    gBaldi.angerTimer = 8.0f;
    gBaldi.speed = BALDI_SPEED_BASE + gPlayer.notebooksCollected * 0.25f +
               gBaldi.angry * 0.45f;
  }
  gShowQuestion = 0;
  gQuestionIdx = -1;
}

/* ============================================================
 *  GLUT CALLBACKS
 * ============================================================ */
int kB = 0; /* B-Soda key */

static void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  switch (gState) {
  case STATE_MENU:
  case STATE_INSTRUCTIONS:
  case STATE_WIN:
  case STATE_LOSE:
    break;
  case STATE_PLAYING:
  case STATE_PAUSED:
    setupCamera();
    renderFloorAndCeiling();
    renderWalls();
    for (int i = 0; i < MAX_NOTEBOOKS; i++)
      renderNotebook(&gNotebooks[i]);
    renderItems();
    renderBaldi();
    renderCloud();
    drawHUD();
    if (gShowQuestion)
      drawQuestion();
    if (gState == STATE_PAUSED)
      drawPause();
    break;
  }

  /* Overlay screens */
  switch (gState) {
  case STATE_MENU:
    drawMenu();
    break;
  case STATE_INSTRUCTIONS:
    drawInstructions();
    break;
  case STATE_WIN:
    drawWin();
    break;
  case STATE_LOSE:
    drawLose();
    break;
  default:
    break;
  }

  glutSwapBuffers();
}

static int gLastTime = 0;
static void idle(void) {
  int now = glutGet(GLUT_ELAPSED_TIME);
  float dt = (now - gLastTime) / 1000.0f;
  if (dt > 0.05f)
    dt = 0.05f;
  gLastTime = now;

  if (gState == STATE_PLAYING) {
    gTime += dt;
    updatePlayer(dt);
    updateBaldi(dt);

    /* Update bsoda cloud */
    if (gCloud.active) {
      gCloud.timeLeft -= dt;
      gCloud.radius += dt * 0.5f;
      if (gCloud.timeLeft <= 0 || gCloud.radius > 4.0f)
        gCloud.active = 0;
    }
  } else if (gState == STATE_MENU || gState == STATE_WIN ||
             gState == STATE_LOSE) {
    gTime += dt;
  }

  glutPostRedisplay();
}

static void reshape(int w, int h) {
  gWindowW = w;
  gWindowH = (h ? h : 1);
  glViewport(0, 0, gWindowW, gWindowH);
}

static void keyboard(unsigned char key, int x, int y) {
  (void)x;
  (void)y;
  switch (key) {
  case 'w':
  case 'W':
    kW = 1;
    break;
  case 'a':
  case 'A':
    kA = 1;
    break;
  case 's':
  case 'S':
    kS = 1;
    break;
  case 'd':
  case 'D':
    kD = 1;
    break;
  case 'e':
  case 'E':
    kE = 1;
    break;
  case ' ':
    kSpace = 1;
    break;
  case 'b':
  case 'B':
    if (gState == STATE_PLAYING && gPlayer.bsodaCount > 0 && !gCloud.active) {
      gPlayer.bsodaCount--;
      /* Fire cloud toward Baldi */
      float yaw_r = gPlayer.yaw * 3.14159f / 180.0f;
      gCloud.x = gPlayer.x + cosf(yaw_r) * 1.5f;
      gCloud.z = gPlayer.z + sinf(yaw_r) * 1.5f;
      gCloud.active = 1;
      gCloud.radius = 0.5f;
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
      glutWarpPointer(gWindowW / 2, gWindowH / 2);
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
  case '1':
    if (gShowQuestion)
      answerQuestion(1);
    break;
  case '2':
    if (gShowQuestion)
      answerQuestion(2);
    break;
  case '3':
    if (gShowQuestion)
      answerQuestion(3);
    break;
  }
  kShift = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? 1 : 0;
}

static void keyboardUp(unsigned char key, int x, int y) {
  (void)x;
  (void)y;
  switch (key) {
  case 'w':
  case 'W':
    kW = 0;
    break;
  case 'a':
  case 'A':
    kA = 0;
    break;
  case 's':
  case 'S':
    kS = 0;
    break;
  case 'd':
  case 'D':
    kD = 0;
    break;
  case 'e':
  case 'E':
    kE = 0;
    break;
  case ' ':
    kSpace = 0;
    break;
  }
  kShift = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? 1 : 0;
}

static void specialKey(int key, int x, int y) {
  (void)x;
  (void)y;
  switch (key) {
  case GLUT_KEY_UP:
    if (gState == STATE_MENU) {
      kW = 1;
      gMenuSel = (gMenuSel + 2) % 3;
    } else
      kUp = 1;
    break;
  case GLUT_KEY_DOWN:
    if (gState == STATE_MENU) {
      gMenuSel = (gMenuSel + 1) % 3;
    } else
      kDown = 1;
    break;
  case GLUT_KEY_LEFT:
    kLeft = 1;
    break;
  case GLUT_KEY_RIGHT:
    kRight = 1;
    break;
  }
  kShift = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? 1 : 0;
}

static void specialKeyUp(int key, int x, int y) {
  (void)x;
  (void)y;
  switch (key) {
  case GLUT_KEY_UP:
    kUp = 0;
    break;
  case GLUT_KEY_DOWN:
    kDown = 0;
    break;
  case GLUT_KEY_LEFT:
    kLeft = 0;
    break;
  case GLUT_KEY_RIGHT:
    kRight = 0;
    break;
  }
  kShift = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? 1 : 0;
}

static void mouseMotion(int x, int y) {
  if (!gMouseCaptured || gState != STATE_PLAYING)
    return;

  int dx = x - gWindowW / 2;
  int dy = y - gWindowH / 2;

  if (dx == 0 && dy == 0)
    return;

  gPlayer.yaw += dx * MOUSE_SENS;
  gPlayer.pitch -= dy * MOUSE_SENS;
  gPlayer.pitch = clampf(gPlayer.pitch, -70, 70);

  glutWarpPointer(gWindowW / 2, gWindowH / 2);
}

static void mouseClick(int button, int state, int x, int y) {
  (void)x;
  (void)y;
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    if (gState == STATE_PLAYING && !gMouseCaptured) {
      glutSetCursor(GLUT_CURSOR_NONE);
      gMouseCaptured = 1;
      glutWarpPointer(gWindowW / 2, gWindowH / 2);
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
  glutCreateWindow("Baldi's ");

  /* OpenGL setup */
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glShadeModel(GL_SMOOTH);
  glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  /* Fog for atmosphere */
  glEnable(GL_FOG);
  GLfloat fogColor[4] = {0.05f, 0.05f, 0.08f, 1.0f};
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_DENSITY, 0.05f);
  glFogi(GL_FOG_MODE, GL_EXP2);

  /* Simple directional lighting */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  GLfloat ambient[] = {0.45f, 0.40f, 0.35f, 1.0f};
  GLfloat diffuse[] = {0.70f, 0.65f, 0.60f, 1.0f};
  GLfloat lightPos[] = {MAP_W * CELL_SIZE / 2, WALL_HEIGHT - 0.1f,
                        MAP_H * CELL_SIZE / 2, 1.0f};
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
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
  // glutPassiveMotionFunc(mouseMotion);
  // glutMotionFunc(mouseMotion);
  // glutMouseFunc(mouseClick);

  printf("=== BALDI'S BASICS 3D ===\n");
  printf("FreeGLUT/OpenGL Game\n");
  printf("Klik window untuk aktifkan mouse, ESC untuk pause\n");

  glutMainLoop();
  return 0;
}