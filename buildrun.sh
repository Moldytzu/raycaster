set -e
gcc -Ofast -g raycaster.c -lGL -lGLU -lglut -lm -o ray
./ray