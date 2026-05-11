#pragma once

/*
 * ============================================================
 *  BALDI'S BASICS 3D - Header
 *  Structs, constants, globals, and function declarations
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
#define WINDOW_W        1280
#define WINDOW_H        720
#define FOV             75.0f
#define MOVE_SPEED      0.08f
#define BALDI_SPEED_BASE 0.025f
#define MOUSE_SENS      0.15f
#define TARGET_FPS      60.0f

#define MAP_W           16
#define MAP_H           16
#define CELL_SIZE       2.0f
#define WALL_HEIGHT     2.5f

#define MAX_NOTEBOOKS   7
#define MAX_ITEMS       5

/* ============================================================
 *  COLORS (RGB 0-1)
 * ============================================================ */
extern const float COL_WALL_A[3];
extern const float COL_WALL_B[3];
extern const float COL_FLOOR[3];
extern const float COL_CEIL[3];
extern const float COL_NOTEBOOK[3];
extern const float COL_BALDI[3];
extern const float COL_STAMINA[3];

/* ============================================================
 *  MAP
 * ============================================================ */
extern int map[MAP_H][MAP_W];

/* ============================================================
 *  GAME STATE ENUM
 * ============================================================ */
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_WIN,
    STATE_LOSE,
    STATE_INSTRUCTIONS
} GameState;

/* ============================================================
 *  STRUCTS
 * ============================================================ */
typedef struct {
    float x, z;
    int   collected;
    int   active;
    float bobOffset;
    int   questionAnswered;
} Notebook;

typedef struct {
    float x, z;
    int   type;   /* 0=energy drink, 1=ruler, 2=bsoda, 3=zesty bar, 4=tape */
    int   active;
} Item;

typedef struct {
    float x, z;
    float angle;
    float speed;
    int   angry;
    float angerTimer;
    int   notebooksCollected;
    float hearRadius;
} Baldi;

typedef struct {
    float x, z;
    float yaw;
    float pitch;
    float stamina;
    int   sprinting;
    int   notebooksCollected;
    int   energyDrinks;
    int   hasRuler;
    int   bsodaCount;
    float interactCooldown;
    float footstepTimer;
} Player;

typedef struct {
    int   active;
    float x, z;
    float radius;
    float timeLeft;
} BsodaCloud;

/* ============================================================
 *  GLOBALS (defined in game.cpp)
 * ============================================================ */
extern GameState   gState;
extern Player      gPlayer;
extern Baldi       gBaldi;
extern Notebook    gNotebooks[MAX_NOTEBOOKS];
extern Item        gItems[MAX_ITEMS];
extern BsodaCloud  gCloud;
extern int         gWindowW;
extern int         gWindowH;
extern float       gTime;
extern int         gMenuSel;
extern float       gFlashTimer;
extern int         gMouseCaptured;

/* Key states (defined in main.cpp) */
extern int kW, kA, kS, kD, kShift, kE, kSpace;
extern int kUp, kDown, kLeft, kRight;
extern int kB;

/* Question popup */
extern int   gShowQuestion;
extern int   gQuestionIdx;
extern int   gAnswerA, gAnswerB, gAnswerC, gCorrectAnswer;
extern char  gQuestionText[64];

/* HUD message */
extern char  gHudMsg[128];
extern float gHudMsgTimer;

/* ============================================================
 *  FUNCTION DECLARATIONS - GAME LOGIC (game.cpp)
 * ============================================================ */
float  randf(float lo, float hi);
float  dist2D(float ax, float az, float bx, float bz);
float  clampf(float v, float lo, float hi);
int    mapAt(int col, int row);
int    mapSolid(int col, int row);
void   worldToCell(float wx, float wz, int *col, int *row);
void   moveWithCollision(float *cx, float *cz, float dx, float dz, float r);

void   initGame(void);
void   updatePlayer(float dt);
void   updateBaldi(float dt);
void   answerQuestion(int choice);

/* ============================================================
 *  FUNCTION DECLARATIONS - RENDERING (game.cpp)
 * ============================================================ */
void   setupCamera(void);
void   renderFloorAndCeiling(void);
void   renderWalls(void);
void   renderNotebook(Notebook *nb);
void   renderPlayer(void);
void   renderBaldi(void);
void   renderItems(void);
void   renderCloud(void);

void   drawHUD(void);
void   drawQuestion(void);
void   drawMenu(void);
void   drawInstructions(void);
void   drawWin(void);
void   drawLose(void);
void   drawPause(void);