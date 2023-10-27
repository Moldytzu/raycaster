SRC="src/raycaster.c src/player.c src/globals.c src/window.c"
LIBS="-lGL -lGLU -lglut -lm"
CFLAGS="-Ofast -g -Isrc/"

set -e # break on error
gcc $CFLAGS $SRC $LIBS -o ray
./ray