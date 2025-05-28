cmake_minimum_required(VERSION 4.0)
# This hash works only with version 4.0.1, needs to be updated according to:
# https://github.com/Kitware/CMake/blob/v4.0.1/Help/dev/experimental.rst
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
set(CMAKE_CXX_MODULE_STD 1)

project(vkCompVizRoot)

add_subdirectory("vkCompViz")

function(addExample NAME DESCRIPTION)
    option(${NAME} ${DESCRIPTION} 1)
    if(${NAME})
        add_subdirectory("examples/${NAME}")
    endif()
endFunction()

addExample("simpleBlending" "A simplest hello-world example that blends two images together")

set(ASTYLE_BINARY "astyle" CACHE PATH "Path to astyle binary to format the code by \"ninja format\"")
set(ASTYLE_OPTIONS -A1 -C -n -k3 -p -U -xg -r -o CACHE STRING "Astyle options used by \"ninja format\"")
add_custom_target(format WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
add_custom_command(TARGET format PRE_BUILD COMMAND ${ASTYLE_BINARY} ${CMAKE_SOURCE_DIR}/*.cpp,*.h,*.cu,*.cxx,*.cppm --exclude=external --exclude=build -i ${ASTYLE_OPTIONS})
