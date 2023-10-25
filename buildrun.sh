set -e
gcc -g raycaster.c -lGL -lGLU -lglut -lm -o ray
./ray