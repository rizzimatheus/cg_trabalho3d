cmake_minimum_required(VERSION 3.17)
project(trabalho3d)

set(CMAKE_CXX_STANDARD 20)

#SET(GCC_COVERAGE_COMPILE_FLAGS "-g -Wall")
#SET(GCC_COVERAGE_LINK_FLAGS    "-lglut -lGL -lGLU")

add_executable(trabalho3d main.cpp tinyxml2.cpp tinyxml2.h player.cpp player.h objloader.cpp objloader.h material.cpp material.h stb_image.h)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})