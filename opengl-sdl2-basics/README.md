## Build on Windows with VC++

### Download SDL2 and SDL2_image
https://github.com/libsdl-org/SDL/releases
https://github.com/libsdl-org/SDL_image/releases

for Build...
- SDL2-devel-2.X.X-VC.zip
- SDL2_image-devel-2.X.X-VC.zip

for Runtime...
- SDL2-2.X.X-win32-x64.zip
- SDL2_image-2.X.X-win32-x64.zip

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
```

### Build
Run cmake
```shell
> cmake -S . -B build
> cmake --build build --config Release
```

### Run
> build\Release\sdl2basics.exe

## Build on Linux with g++

### Install SDL2 and SDL2_image
```shell
sudo apt install libsdl2-dev
sudo apt install libsdl2-image-dev
```
OpenGL headers are included in graphics environment on Linux

### Build
Run cmake
```shell
> cmake -S . -B build
> cmake --build build --config Release
```

### Run
> build/sdl2basics
