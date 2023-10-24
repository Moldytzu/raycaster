#include <GL/glut.h>
#include <math.h>

#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH (WINDOW_HEIGHT*2)
#define PI 3.14159265358979323846264338327950288
#define WRAP_AROUND_RADIANS(x) {if(x < 0) x += 2*PI; if(x > 2*PI) x -= 2 * PI;} // wrap x degree around radians values (0<=x<=2PI)

double playerX = 400, playerY = 400, playerDx, playerDy, playerAngle /*in radians*/, playerSize2D = 10, playerSpeed = 5;

const int mapWidth = 8, mapHeight = 8, mapArea = mapWidth * mapHeight, mapSize2D = WINDOW_HEIGHT/mapHeight;
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

void mapDraw2D()
{
    for(int x = 0; x < mapWidth; x++)
    {
        for(int y = 0; y < mapHeight; y++)
        {
            int xOffset = x * mapSize2D, yOffset = y * mapSize2D;
            int wallState = mapWalls[y * mapWidth + x];

            if(wallState == 1) 
                glColor3f(0,0,0); // each wall is black
            else
                glColor3f(1,1,1); // each empty is white

            // draw a square at the points
            glBegin(GL_QUADS);
            glVertex2i(xOffset, yOffset);
            glVertex2i(xOffset, yOffset + mapSize2D);
            glVertex2i(xOffset + mapSize2D, yOffset + mapSize2D);
            glVertex2i(xOffset + mapSize2D, yOffset);
            glEnd();
        }
    }
}

void playerDraw()
{
    // draw point
    glColor3f(0, 0, 1); // blue
    glPointSize(playerSize2D);
    glBegin(GL_POINTS);
    glVertex2f(playerX, playerY);
    glEnd();

    // draw orientation
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2i(playerX, playerY);
    glVertex2i(playerX + playerDx * 10, playerY + playerDy * 10);
    glEnd();
}

// Callbacks
void windowUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen
    mapDraw2D();                                        // draw the map
    playerDraw();                                       // draw the player
    glutSwapBuffers();                                  // update the screen
}

void windowKeyboard(char key, int x, int y)
{
    switch (key)
    {
    case 'a':
        playerAngle += 0.1;
        WRAP_AROUND_RADIANS(playerAngle);
        playerUpdateDelta();
        break;

    case 'd':
        playerAngle -= 0.1;
        WRAP_AROUND_RADIANS(playerAngle);
        playerUpdateDelta();
        break;

    case 's':
        // do the vector addition and subtraction
        playerY -= playerDy;
        playerX -= playerDx;
        break;

    case 'w':
        // do the vector addition and subtraction
        playerY += playerDy;
        playerX += playerDx;
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