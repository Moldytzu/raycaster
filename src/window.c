#include <raycaster.h>

#define TEXTURE_WIDTH 10
#define TEXTURE_HEIGHT 10
double wallTexture[TEXTURE_WIDTH][TEXTURE_HEIGHT] = {
    {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9},
    {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9},
    {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9},
    {0, 0.1, 0.2, 0.3, 0.4, 0.0, 0.6, 0.7, 0.8, 0.9},
    {0, 0.1, 0.2, 0.3, 0.4, 0.0, 0.6, 0.7, 0.8, 0.9},
    {0, 0.1, 0.2, 0.3, 0.4, 0.0, 0.6, 0.7, 0.8, 0.9},
    {0, 0.1, 0.2, 0.3, 0.0, 0.0, 0.0, 0.7, 0.8, 0.9},
    {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9},
    {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9},
    {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9},
};

__attribute__((always_inline)) inline static void castWalls()
{
    double rayAngle, wallX, wallY, rayX, rayY, distance, lineHeight, lineOffset, cameraAngle, shadeDistance, shading;
    int wallTextureX = 0, wallTextureY = 0;
    bool hitRight, hitLeft, hitFront, hitBack;

    for (double ray = 0; ray < FOV * RAY_PER_DEG; ray++) // we want to raycast FOV * RAYS_PER_DEG rays
    {
        rayAngle = playerXAngle + TO_RADIANS(ray / RAY_PER_DEG - FOV / 2); // set the ray angle derived from the ray index
        wallX = playerX;                                                   // set current wall coordinates to player's
        wallY = playerY;                                                   //
        rayY = sin(rayAngle) * DISTANCE_COEFFICIENT;                       // use vector decomposition to determine X and Y components of the ray
        rayX = cos(rayAngle) * DISTANCE_COEFFICIENT;                       //

        for (distance = 0; distance < MAX_DISTANCE; distance++) // check for ray collision
        {
            wallX += rayX; // increase wall coordinates
            wallY += rayY;

            bool inMap = (int)wallX < mapWidth && (int)wallY < mapHeight && (int)wallX >= 0 && (int)wallY >= 0; // check for wall boundaries
            if (!inMap)
                continue;

            if (mapWalls[(int)wallY][(int)wallX]) // check for wall to be present
            {
                // if wallX coordinate is very close to being an integer, it means it's very close to the edge and we're just casting in the side
                hitRight = FRACTIONAL_OF(wallX) > 0.99;
                hitLeft = FRACTIONAL_OF(wallX) < 0.01;

                // same thing, but for the back and front
                hitFront = FRACTIONAL_OF(wallY) > 0.99;
                hitBack = FRACTIONAL_OF(wallY) < 0.01;

                if (hitRight || hitLeft)                       // if we've hit the side
                    wallTextureX = FRACTIONAL_OF(wallY) * 100; // we will use the Y fraction as the X index
                else                                           // or else if we've hit the front/back
                    wallTextureX = FRACTIONAL_OF(wallX) * 100; // we will use th X fraction as the X index

                wallTextureX /= TEXTURE_WIDTH;
                break;
            }
        }

        // fisheye compensation
        cameraAngle = playerXAngle - rayAngle; // determine the camera angle
        WRAP_AROUND_RADIANS(cameraAngle);
        distance = distance * cos(cameraAngle); // adjust distance by x component of camera angle

        lineHeight = WALL_HEIGHT * MAX_WALL_HEIGHT / distance;
        lineOffset = WALL_HEIGHT - lineHeight / 2 + WINDOW_HEIGHT * cos(playerYAngle); // move the line at middle and modify its offset based on the player vertical angle

        // draw the ray on the map
        shadeDistance = 1 / distance;
        shading = shadeDistance * SHADE_COEFFICIENT;

        if (shading >= 1) // clamp the shade to 1
            shading = 1;

        // wall drawing
        double z = lineHeight / TEXTURE_HEIGHT;
        for (double drawY = 0; drawY < TEXTURE_HEIGHT; drawY++)
        {
            wallTextureY = TEXTURE_HEIGHT - 1 - (int)drawY;
            glColor3d(wallTexture[wallTextureY][wallTextureX] * shading, 0, 0);
            glVertex2d(ray / RESOLUTION, drawY * z + lineOffset - FLOOR_HIDE_COEFFICIENT /*hides floor mapping inaccuracies*/);
            glVertex2d(ray / RESOLUTION, (drawY + 1) * z + lineOffset - FLOOR_HIDE_COEFFICIENT /*hides floor mapping inaccuracies*/);
        }
    }
}

void castFloorAndCeiling()
{
    double rayAngle, tileX, tileY, rayX, rayY, distance, lineHeight, lineOffset, cameraAngle, shadeDistance, shading, lastLineOffset = 0.0001;
    int wallTextureX = 0, wallTextureY = 0;
    bool hitRight, hitLeft, hitFront, hitBack;

    for (double ray = 0; ray < FOV * RAY_PER_DEG; ray++) // we want to raycast FOV * RAYS_PER_DEG rays
    {
        rayAngle = playerXAngle + TO_RADIANS(ray / RAY_PER_DEG - FOV / 2); // set the ray angle derived from the ray index
        tileX = playerX;                                                   // set current wall coordinates to player's
        tileY = playerY;                                                   //
        rayY = sin(rayAngle) * DISTANCE_COEFFICIENT * FLOOR_COEFFICIENT;   // use vector decomposition to determine X and Y components of the ray
        rayX = cos(rayAngle) * DISTANCE_COEFFICIENT * FLOOR_COEFFICIENT;   //

        bool inMap = true;
        distance = 0;

        // cast the ray until the map ends or we encounter a wall
        for (;;)
        {
            tileX += rayX; // increase wall coordinates
            tileY += rayY;

            inMap = (int)tileX < mapWidth && (int)tileY < mapHeight && (int)tileX >= 0 && (int)tileY >= 0;

            if (mapWalls[(int)tileY][(int)tileX] || !inMap)
                break;

            distance++;

            // calculate texture coordinates
            wallTextureX = FRACTIONAL_OF(tileX) * 100;
            wallTextureX /= TEXTURE_WIDTH;

            wallTextureY = FRACTIONAL_OF(tileY) * 100;
            wallTextureY /= TEXTURE_WIDTH;

            // fisheye compensation
            cameraAngle = playerXAngle - rayAngle; // determine the camera angle
            WRAP_AROUND_RADIANS(cameraAngle);
            double correctedDistance = distance * cos(cameraAngle); // adjust distance by x component of camera angle

            lineHeight = WALL_HEIGHT * MAX_WALL_HEIGHT / correctedDistance;
            lineOffset = WALL_HEIGHT - lineHeight / (2 * FLOOR_COEFFICIENT) + WINDOW_HEIGHT * cos(playerYAngle); // move the line at middle and modify its offset based on the player vertical angle

            // draw the ray on the map
            shadeDistance = 1 / correctedDistance;
            shading = shadeDistance * SHADE_COEFFICIENT / FLOOR_COEFFICIENT;

            if (shading >= 1) // clamp the shade to 1
                shading = 1;

            // draw floor
            glColor3d(0, shading * wallTexture[wallTextureY][wallTextureX], 0);
            glVertex2d(ray / RESOLUTION, lineOffset);
            glVertex2d(ray / RESOLUTION, lineOffset - FLOOR_COEFFICIENT * fabs(lineOffset - lastLineOffset));

            // draw ceiling
            glColor3d(0, 0, shading * wallTexture[wallTextureY][wallTextureX]);
            glVertex2d(ray / RESOLUTION, WINDOW_HEIGHT - lineOffset);
            glVertex2d(ray / RESOLUTION, lineOffset + FLOOR_COEFFICIENT * fabs(lineOffset - lastLineOffset));

            lastLineOffset = lineOffset;
        }
    }
}

void windowPaint()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINES);
    castFloorAndCeiling(); // do floor and ceiling casting
    castWalls();           // do wall casting
    glEnd();
    glutSwapBuffers(); // update the screen
}

void windowUpdate()
{
    int a = glutGet(GLUT_ELAPSED_TIME), b;

    playerUpdate();
    windowPaint();

    do
    {
        b = glutGet(GLUT_ELAPSED_TIME);
        deltaTime = (b - a) * 1 / 1000.0;
    } while (b - a <= 0 || ((1 / deltaTime) > MAX_FPS && MAX_FPS));

    printf("FPS: %f\n", 1 / deltaTime);

    glutPostRedisplay(); // redraw window
}

void windowKeyboardUp(char key, int x, int y)
{
    switch (key)
    {
    case 'd':
        keyLookRight = false;
        break;
    case 'a':
        keyLookLeft = false;
        break;
    case 's':
        keyWalkBackwards = false;
        break;
    case 'w':
        keyWalkForwards = false;
        break;
    case 'i':
        keyLookUp = false;
        break;
    case 'k':
        keyLookDown = false;
        break;
    default:
        break;
    }

    glutPostRedisplay(); // tell glut we're done
}

void windowKeyboard(char key, int x, int y)
{
    switch (key)
    {
    case 'd':
        keyLookRight = true;
        break;
    case 'a':
        keyLookLeft = true;
        break;
    case 's':
        keyWalkBackwards = true;
        break;
    case 'w':
        keyWalkForwards = true;
        break;
    case 'i':
        keyLookUp = true;
        break;
    case 'k':
        keyLookDown = true;
        break;
    default:
        break;
    }

    glutPostRedisplay(); // tell glut we're done
}

void windowInit()
{
    // create a window with glut
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("glut");

    // set up callbacks
    glutDisplayFunc(windowUpdate);
    glutKeyboardFunc(windowKeyboard);
    glutKeyboardUpFunc(windowKeyboardUp);

    // set up opengl context
    glClearColor(0.6, 0.6, 0.6, 0);                // gray background
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT); // set up projection
    glLineWidth(1 / RESOLUTION);
}