cmake_minimum_required(VERSION 3.10)
project(Flint_and_Pixels_3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL3 REQUIRED)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/math3d.cpp
    src/renderer.cpp
    src/mesh.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
