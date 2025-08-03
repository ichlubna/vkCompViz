cmake_minimum_required(VERSION 4.0)
set(CMAKE_COLOR_DIAGNOSTICS ON)
include(functions.cmake)
# This hash works only with version 4.0.1, needs to be updated according to:
# https://github.com/Kitware/CMake/blob/v4.0.1/Help/dev/experimental.rst
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
set(CMAKE_CXX_MODULE_STD 1)

project(vkCompVizRoot)
add_subdirectory("vkCompViz")

addExample("simpleBlending" "Simple blending of two images using textures")
addExample("parallelReduction" "Average value of an array using global and local memory, multiple passes and benchmarks")
addExample("3DViewer" "Renders a simple 3D model")
