cmake_minimum_required(VERSION 3.14)
#include (CMakePrintSystemInformation)
#set(CMAKE_CONFIGURATION_TYPES Debug Release)
#if(APPLE)
#  set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for macOS" FORCE)
#endif()
# enable_testing()

project(sdl2basics VERSION 1.0.0 LANGUAGES CXX C)
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
find_package(SDL2_image REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} main.c)

#if(MSVC)
#  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
#endif()

target_link_libraries(${PROJECT_NAME} 
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    OpenGL::GL
)

## If you want to build the Release version, type the following command
#set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
