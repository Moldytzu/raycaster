#include <raycaster.h>

void playerUpdateDelta()
{
    playerDx = cos(playerXAngle) * playerSpeed; // x component of player angle
    playerDy = sin(playerXAngle) * playerSpeed; // y component of player angle
    // becuase playerXAngle is a normalised vector, we can use playerSpeed as a scaler
}

void playerUpdate()
{
    if (keyLookRight)
    {
        playerXAngle += TO_RADIANS(playerRotationDegrees * deltaTime);
        WRAP_AROUND_RADIANS(playerXAngle);
        playerUpdateDelta();
    }

    if (keyLookLeft)
    {
        playerXAngle -= TO_RADIANS(playerRotationDegrees * deltaTime);
        WRAP_AROUND_RADIANS(playerXAngle);
        playerUpdateDelta();
    }

    if (keyWalkBackwards)
    {
        // do the vector addition and subtraction
        playerY -= playerDy * playerSpeed * deltaTime;
        playerX -= playerDx * playerSpeed * deltaTime;
    }

    if (keyWalkForwards)
    {
        // do the vector addition and subtraction
        playerY += playerDy * playerSpeed * deltaTime;
        playerX += playerDx * playerSpeed * deltaTime;
    }

    if (keyLookUp)
    {
        playerYAngle += TO_RADIANS(playerRotationDegrees * deltaTime);
        if (playerYAngle >= PI)
            playerYAngle = PI;

        playerDz = cos(playerYAngle);
    }

    if (keyLookDown)
    {
        playerYAngle -= TO_RADIANS(playerRotationDegrees * deltaTime);
        if (playerYAngle <= 0)
            playerYAngle = 0;

        playerDz = cos(playerYAngle);
    }
}