#include <raycaster.h>

int main(int argc, char **argv)
{
    glutInit(&argc, argv); // initialise glut
    windowInit();          // create the window
    playerUpdateDelta();   // generate initial player x and y delta
    glutMainLoop();        // run the program
    return 0;
}