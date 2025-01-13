echo SDL2_DIR=%SDL2_DIR%
echo SDL2_IMAGE_DIR=%SDL2_IMAGE_DIR%

cl main.c -I%SDL2_DIR%\include -I%SDL2_IMAGE_DIR%\include shell32.lib SDL2.lib SDL2main.lib SDL2_image.lib OPENGL32.lib /link /LIBPATH:%SDL2_DIR%\lib\x64 /LIBPATH:%SDL2_IMAGE_DIR%\lib\x64 /Subsystem:Console
