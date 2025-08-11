cmake_minimum_required(VERSION 4.0)
set(CMAKE_COLOR_DIAGNOSTICS ON)
include(functions.cmake)
# This hash works only with the given cmake version:
# https://github.com/Kitware/CMake/blob/v4.1.0/Help/dev/experimental.rst
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD 1)

project(vkCompVizRoot)
add_subdirectory("vkCompViz")

addExample("simpleBlending" "Simple blending of two images using textures")
addExample("parallelReduction" "Average value of an array using global and local memory, multiple passes and benchmarks")
addExample("3DViewer" "Renders a simple 3D model")
