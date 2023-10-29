#include <raycaster.h>

double playerX = 1, playerY = 1, playerDx, playerDy, playerDz, playerYAngle = 1.5707 /*90 deg in radians*/, playerXAngle = 0.523 /*30 deg in radians*/, playerSpeed = 1.5, playerRotationDegrees = 100;
double deltaTime = 0;
bool keyWalkForwards = false, keyWalkBackwards = false, keyLookLeft = false, keyLookRight = false, keyLookUp = false, keyLookDown = false;
