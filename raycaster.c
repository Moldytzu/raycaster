#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

// NOTE: all angles are in RADIANS!

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define PI M_PI
#define WRAP_AROUND_RADIANS(x) \
    {                          \
        if (x < 0)             \
            x += 2 * PI;       \
        if (x > 2 * PI)        \
            x -= 2 * PI;       \
    } // wrap x degree around radians values (0<=x<=2PI)

double playerX = 1, playerY = 1, playerDx, playerDy, playerAngle = 0.523 /*30 deg in radians*/, playerSpeed = 1.5, playerRotationDegrees = 100;
double deltaTime = 0;

const int mapWidth = 8, mapHeight = 8;
int mapWalls[] = {
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,
    1,0,0,1,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,1,0,1,
    1,0,1,0,0,1,0,1,
    1,0,1,0,0,1,0,1,
    1,1,1,1,1,1,1,1,
};

// Update/Draw
void playerUpdateDelta()
{
    playerDx = cos(playerAngle) * playerSpeed; // x component of player angle
    playerDy = sin(playerAngle) * playerSpeed; // y component of player angle
    // becuase playerAngle is a normalised vector, we can use playerSpeed as a scaler
}

// helpers
#define TO_RADIANS(x) ((x) * (PI / 180.0))
#define FRACTIONAL_OF(x) ((x)-(int)(x))

// settings
#define FOV 60.0 // in degrees
#define RESOLUTION 10
#define MAX_DISTANCE 10000
#define RAY_PER_DEG (WINDOW_WIDTH / FOV * RESOLUTION)
#define DISTANCE_COEFFICIENT (0.005 / TO_RADIANS(FOV))
#define SHADE_COEFFICIENT 400
#define MAX_WALL_HEIGHT WINDOW_HEIGHT
#define WALL_HEIGHT (MAX_WALL_HEIGHT / 2)
#define MAX_FPS 1000

#define TEXTURE_WIDTH 10
#define TEXTURE_HEIGHT 10
double wallTexture[TEXTURE_WIDTH][TEXTURE_HEIGHT] = {
    {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9},
    {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9},
    {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9},
    {0,0.1,0.2,0.3,0.4,0.0,0.6,0.7,0.8,0.9},
    {0,0.1,0.2,0.3,0.4,0.0,0.6,0.7,0.8,0.9},
    {0,0.1,0.2,0.3,0.4,0.0,0.6,0.7,0.8,0.9},
    {0,0.1,0.2,0.3,0.0,0.0,0.0,0.7,0.8,0.9},
    {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9},
    {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9},
    {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9},
};

void drawRays()
{
    double rayAngle, wallX, wallY, rayX, rayY, h, distance, lineHeight, lineOffset, cameraAngle, shadeDistance, shading;
    int wallTextureX, wallTextureY;
    bool hitRight, hitLeft; 
    for (double ray = 0; ray < FOV * RAY_PER_DEG; ray++) // we want to raycast FOV * RAYS_PER_DEG rays
    {
        rayAngle = playerAngle + TO_RADIANS(ray / RAY_PER_DEG - FOV / 2); // set the ray angle derived from the ray index
        wallX = playerX; // set current wall coordinates to player's
        wallY = playerY; //
        rayY = sin(rayAngle) * DISTANCE_COEFFICIENT; // use vector decomposition to determine X and Y components of the ray
        rayX = cos(rayAngle) * DISTANCE_COEFFICIENT; //

        for(distance = 0; distance < MAX_DISTANCE; distance++) // check for ray collision
        {
            wallX = wallX + rayX; // increase wall coordinates
            wallY = wallY + rayY;

            if (wallX < mapWidth && wallY < mapHeight &&       // check for wall boundaries
                wallX >= 0 && wallY >= 0 &&
                mapWalls[(int)wallY * mapHeight + (int)wallX]) // check for wall to be present
            {
                // if wallX coordinate is very close to being an integer, it means it's very close to the edge and we're just casting in the side 
                hitRight = FRACTIONAL_OF(wallX) > 0.99;
                hitLeft = FRACTIONAL_OF(wallX) < 0.01;  

                if(hitRight || hitLeft)                        // if we've hit the side
                    wallTextureX = FRACTIONAL_OF(wallY) * 100; // we will use the Y fraction as the X index
                else                                           // or else if we've hit the front/back
                    wallTextureX = FRACTIONAL_OF(wallX) * 100; // we will use th X fraction as the X index

                wallTextureX /= TEXTURE_WIDTH;
                break;
            }
        }

        // fisheye compensation
        cameraAngle = playerAngle - rayAngle; // determine the camera angle
        WRAP_AROUND_RADIANS(cameraAngle);
        distance = distance * cos(cameraAngle); // adjust distance by x component of camera angle

        lineHeight = WALL_HEIGHT * MAX_WALL_HEIGHT / distance;
        lineOffset = WALL_HEIGHT - lineHeight/2; // move the line at middle

        // draw the ray on the map
        shadeDistance = 1 / distance;
        shading = shadeDistance * SHADE_COEFFICIENT;

        if(shading >= 1) // clamp the shade to 1
            shading = 1;

        glLineWidth(1 / RESOLUTION);
        glBegin(GL_LINES);

        // wall drawing    
        double z = lineHeight / TEXTURE_HEIGHT;
        for(double drawY = 0; drawY < TEXTURE_HEIGHT; drawY++)
        {
            wallTextureY = TEXTURE_HEIGHT - 1 - (int)drawY;
            glColor3f(wallTexture[wallTextureY][wallTextureX] * shading,0,0);
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
}

// Callbacks
void windowUpdate()
{
    int a =  glutGet(GLUT_ELAPSED_TIME);

    drawRays();        // draw rays on screen
    glutSwapBuffers(); // update the screen

    do
    {
        int b = glutGet(GLUT_ELAPSED_TIME);
        deltaTime = (b-a) * 1/1000.0; 
    } while((1/deltaTime) > MAX_FPS);

    printf("FPS: %f\n",1/deltaTime);

    glutPostRedisplay(); // redraw window
}

void windowKeyboard(char key, int x, int y)
{
    switch (key)
    {
    case 'd':
        playerAngle += TO_RADIANS(playerRotationDegrees * deltaTime);
        WRAP_AROUND_RADIANS(playerAngle);
        playerUpdateDelta();
        break;

    case 'a':
        playerAngle -= TO_RADIANS(playerRotationDegrees * deltaTime);
        WRAP_AROUND_RADIANS(playerAngle);
        playerUpdateDelta();
        break;

    case 's':
        // do the vector addition and subtraction
        playerY -= playerDy * playerSpeed * deltaTime;
        playerX -= playerDx * playerSpeed * deltaTime;
        break;

    case 'w':
        // do the vector addition and subtraction
        playerY += playerDy * playerSpeed * deltaTime;
        playerX += playerDx * playerSpeed * deltaTime;
        break;

    default:
        break;
    }

    glutPostRedisplay(); // tell glut we're done
}

// Initialisation
void windowInit()
{
    // create a window with glut
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("glut");

    // set up callbacks
    glutDisplayFunc(windowUpdate);
    glutKeyboardFunc(windowKeyboard);
}

void graphicsInit()
{
    glClearColor(0.6, 0.6, 0.6, 0);                // gray background
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT); // set up projection
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv); // initialise glut
    windowInit();          // create the window
    graphicsInit();        // initialise opengl
    playerUpdateDelta();   // generate initial player x and y delta
    glutMainLoop();        // run the program
    return 0;
}