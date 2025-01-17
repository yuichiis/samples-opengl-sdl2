cmake_minimum_required(VERSION 3.14)
#include (CMakePrintSystemInformation)
#set(CMAKE_CONFIGURATION_TYPES Debug Release)
#if(APPLE)
#  set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for macOS" FORCE)
#endif()
# enable_testing()

project(sdl2glsl VERSION 1.0.0 LANGUAGES CXX C)
#set(PROJECT_VERSION_MAJOR 1)
#set(PROJECT_VERSION_MINOR 0)
#set(PROJECT_VERSION_PATCH 0)
message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(MSVC)
  set(SDL2_DIR $ENV{SDL2_DIR})
  if (NOT SDL2_DIR)
      message(FATAL_ERROR "SDL2_DIR environment variable is not set!")
  endif()
  set(SDL2_IMAGE_DIR $ENV{SDL2_IMAGE_DIR})
  if (NOT SDL2_DIR)
      message(FATAL_ERROR "SDL2_DIR environment variable is not set!")
  endif()
  if (NOT SDL2_IMAGE_DIR)
      message(FATAL_ERROR "SDL2_IMAGE_DIR environment variable is not set!")
  endif()
  list(APPEND CMAKE_PREFIX_PATH ${SDL2_DIR}/cmake)
  list(APPEND CMAKE_PREFIX_PATH ${SDL2_IMAGE_DIR}/cmake)
  message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

endif()

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

if(MSVC)
    find_package(SDL2_image REQUIRED)
    set(GLEW_DIR $ENV{GLEW_DIR})
    if (NOT GLEW_DIR)
        message(FATAL_ERROR "GLEW_DIR environment variable is not set!")
    endif()
  
    set(GLEW_INCLUDE_DIRS "${GLEW_DIR}/include")
    set(_glew_library     "${GLEW_DIR}/lib/Release/x64/glew32.lib")
    set(_glew_dll_library "${GLEW_DIR}/bin/Release/x64/glew32.dll")
    if(EXISTS "${_glew_library}" AND EXISTS "${_glew_dll_library}")
        if(NOT TARGET GLEW::GLEW)
            add_library(GLEW::GLEW SHARED IMPORTED)
            set_target_properties(GLEW::GLEW
                PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIRS}"
                    IMPORTED_IMPLIB "${_glew_library}"
                    IMPORTED_LOCATION "${_glew_dll_library}"
                    COMPATIBLE_INTERFACE_BOOL "GLEW_SHARED"
                    INTERFACE_GLEW_SHARED "ON"
                    COMPATIBLE_INTERFACE_STRING "GLEW_VERSION"
                    INTERFACE_GLEW_VERSION "GLEW"
            )
        endif()
        set(GLEW_GLEW_FOUND TRUE)
        message(STATUS "GLEW Found")
    else()
        set(GLEW_GLEW_FOUND FALSE)
        message(STATUS "GLEW Not Found")
    endif()
    unset(_glew_library)
    unset(_glew_dll_library)
    
    set(SDL2_IMAGE_LIBS SDL2_image::SDL2_image)
    set(OPENGL_LIBS OpenGL::GL GLEW::GLEW)
elseif(UNIX)
    # Use pkg-config to locate SDL2_image
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    # Include directories for SDL2_image
    include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
    link_directories(${SDL2_IMAGE_LIBRARY_DIRS})
    # Add libraries to link
    set(SDL2_IMAGE_LIBS ${SDL2_IMAGE_LIBRARIES})
    set(OPENGL_LIBS OpenGL::GL)
endif()
message(STATUS "OPENGL_LIBS: ${OPENGL_LIBS}")

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} 
    SDL2::SDL2
    SDL2::SDL2main
    ${SDL2_IMAGE_LIBS}
    ${OPENGL_LIBS}
)

## If you want to build the Release version, type the following command
#set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
