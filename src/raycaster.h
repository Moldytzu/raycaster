#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

// globals
extern double playerX, playerY, playerDx, playerDy, playerYAngle, playerXAngle, playerSpeed, playerRotationDegrees;
extern double deltaTime;
extern bool keyWalkForwards, keyWalkBackwards, keyLookLeft, keyLookRight, keyLookUp, keyLookDown;

const static int mapWidth = 8, mapHeight = 8;
const static int mapWalls[8][8] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
};

// NOTE: all angles are in RADIANS!

// settings
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define FOV 75.0 // in degrees
#define RESOLUTION 4
#define MAX_DISTANCE 10000
#define RAY_PER_DEG (WINDOW_WIDTH / FOV * RESOLUTION)
#define DISTANCE_COEFFICIENT (0.005 / TO_RADIANS(FOV))
#define SHADE_COEFFICIENT 300
#define MAX_WALL_HEIGHT WINDOW_HEIGHT
#define WALL_HEIGHT (MAX_WALL_HEIGHT / 2)
#define MAX_FPS 0

// Helpers
#define TO_RADIANS(x) ((x) * (PI / 180.0))
#define FRACTIONAL_OF(x) ((x) - (int)(x))
#define PI M_PI
#define WRAP_AROUND_RADIANS(x) \
    {                          \
        if (x < 0)             \
            x += 2 * PI;       \
        if (x > 2 * PI)        \
            x -= 2 * PI;       \
    } // wrap x degree around radians values (0<=x<=2PI)
#define CLAMP_TO_RADIANS(x) \
    {                       \
        if (x < 0)          \
            x = 0;          \
        if (x > 2 * PI)     \
            x = 2 * PI;     \
    }

// Player
void playerUpdateDelta();
void playerUpdate();

// Window
void windowInit();