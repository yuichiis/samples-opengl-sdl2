# debian
# sudo apt install libsdl2-devui
# ubuntu
# sudo apt install build-essential autoconf automake libtool bison re2c pkg-config
# sudo apt install libsdl2-dev
# sudo apt install libsdl2-image-dev
SDL2LIBS=`pkg-config sdl2 --libs`
SDL2FLAGS=`pkg-config sdl2 --cflags`
SDL2LIBS_IMAGE=`pkg-config SDL2_image --libs`

gcc $SDL2FLAGS main.c -lOpenGL $SDL2LIBS $SDL2LIBS_IMAGE -o main.out
