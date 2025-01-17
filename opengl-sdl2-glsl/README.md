# Basic Shader Sample code of OpenGL on SDL2

## Build on Windows with VC++

### Download SDL2 and SDL2_image and glew
GLEW is The OpenGL Extension Wrangler Library.
GLEW is NOT Framework. Just a loader library for OpenGL Extension.

Download from ...
- https://github.com/libsdl-org/SDL/releases
- https://github.com/libsdl-org/SDL_image/releases
- https://sourceforge.net/projects/glew/files/

for Build...
- SDL2-devel-2.X.X-VC.zip
- SDL2_image-devel-2.X.X-VC.zip
- glew-2.2.0-win32.zip

for Runtime...
- SDL2-2.X.X-win32-x64.zip
- SDL2_image-2.X.X-win32-x64.zip
- (glew-2.2.0-win32.zip)

OpenGL headers are included in VC++(Community Edition)

### Extract SDL libs
Extract to C:\SDL2

### Set DLL Path
Add Runtime DLL's PATHs
```shell
path %PATH%;C:\SDL2\****\bin
```

### Set cmake Package's Path
Set SDL2 devel Path
```shell
set SDL2_DIR=C:\SDL2\SDL2-2.X.X
set SDL2_IMAGE_DIR=C:\SDL2\SDL2_image-2.X.X
set GLEW=C:\GLEW\glew-2.x.x-win32-x64\glew-2.X.X
```

### Build
Run cmake
```shell
> cmake -S . -B build
> cmake --build build --config Release
```

### Run
```shell
> build\Release\sdl2basics.exe
```

## Build on Linux with gcc

### Install SDL2 and SDL2_image
```shell
$ sudo apt install libsdl2-dev
$ sudo apt install libsdl2-image-dev
$ sudo apt install libglew-dev
```
OpenGL headers are included in graphics environment on Linux

### Build
Run cmake
```shell
$ cmake -S . -B build
$ cmake --build build --config Release
```

### Run
```shell
$ build/sdl2glsl
```
