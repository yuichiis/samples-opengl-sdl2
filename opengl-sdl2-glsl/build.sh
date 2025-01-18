# debian
# sudo apt install libsdl2-devui
# ubuntu
# sudo apt install build-essential autoconf automake libtool bison re2c pkg-config
# sudo apt install libsdl2-dev
# sudo apt install libsdl2-image-dev
SDL2LIBS=`pkg-config sdl2 --libs`
SDL2FLAGS=`pkg-config sdl2 --cflags`
GLEWLIBS=`pkg-config glew --libs`

gcc $SDL2FLAGS main.c -lOpenGL -lm $SDL2LIBS $GLEWLIBS -o main.out
