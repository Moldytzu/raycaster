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

void windowPaint()
{
    double rayAngle, wallX, wallY, rayX, rayY, h, distance, lineHeight, lineOffset, cameraAngle, shadeDistance, shading;
    int wallTextureX, wallTextureY;
    bool hitRight, hitLeft;
    for (double ray = 0; ray < FOV * RAY_PER_DEG; ray++) // we want to raycast FOV * RAYS_PER_DEG rays
    {
        rayAngle = playerXAngle + TO_RADIANS(ray / RAY_PER_DEG - FOV / 2); // set the ray angle derived from the ray index
        wallX = playerX;                                                   // set current wall coordinates to player's
        wallY = playerY;                                                   //
        rayY = sin(rayAngle) * DISTANCE_COEFFICIENT;                       // use vector decomposition to determine X and Y components of the ray
        rayX = cos(rayAngle) * DISTANCE_COEFFICIENT;                       //

        for (distance = 0; distance < MAX_DISTANCE; distance++) // check for ray collision
        {
            wallX = wallX + rayX; // increase wall coordinates
            wallY = wallY + rayY;

            if ((int)wallX < mapWidth && (int)wallY < mapHeight && // check for wall boundaries
                (int)wallX >= 0 && (int)wallY >= 0 &&
                mapWalls[(int)wallY][(int)wallX]) // check for wall to be present
            {
                // if wallX coordinate is very close to being an integer, it means it's very close to the edge and we're just casting in the side
                hitRight = FRACTIONAL_OF(wallX) > 0.99;
                hitLeft = FRACTIONAL_OF(wallX) < 0.01;

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

        glLineWidth(1 / RESOLUTION);
        glBegin(GL_LINES);

        // wall drawing
        double z = lineHeight / TEXTURE_HEIGHT;
        for (double drawY = 0; drawY < TEXTURE_HEIGHT; drawY++)
        {
            wallTextureY = TEXTURE_HEIGHT - 1 - (int)drawY;
            glColor3f(wallTexture[wallTextureY][wallTextureX] * shading, 0, 0);
            glVertex2f(ray / RESOLUTION, drawY * z + lineOffset);
            glVertex2f(ray / RESOLUTION, (drawY + 1) * z + lineOffset);
        }

        // floor drawing
        glColor3f(0, shading, 0);
        glVertex2f(ray / RESOLUTION, lineOffset);
        glVertex2f(ray / RESOLUTION, 0);

        // ceiling
        glColor3f(0, 0, shading * 0.8);
        glVertex2f(ray / RESOLUTION, WINDOW_HEIGHT);
        glVertex2f(ray / RESOLUTION, lineHeight + lineOffset);

        glEnd();
    }

    glutSwapBuffers(); // update the screen
}

void windowUpdate()
{
    int a = glutGet(GLUT_ELAPSED_TIME);

    playerUpdate();
    windowPaint();

    do
    {
        int b = glutGet(GLUT_ELAPSED_TIME);
        deltaTime = (b - a) * 1 / 1000.0;
    } while ((1 / deltaTime) > MAX_FPS && MAX_FPS);

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
}