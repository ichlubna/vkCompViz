cmake_minimum_required(VERSION 4.0)
project(vkCompVizRoot)
add_subdirectory("vkCompViz")

function(addExample NAME DESCRIPTION)
    option(${NAME} ${DESCRIPTION} 1)
    if(${NAME})
        add_subdirectory("examples/${NAME}")
    endif()
endFunction()

addExample("simpleBlending" "A simplest hello-world example that blends two images together")

set(ASTYLE_BINARY "astyle" CACHE PATH "Path to astyle binary to format the code by \"make format\"")
set(ASTYLE_OPTIONS -A1 -C -n -k3 -p -U -xg -r CACHE STRING "Astyle options used by \"make format\"")
add_custom_target(format WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
add_custom_command(TARGET format PRE_BUILD COMMAND ${ASTYLE_BINARY} ${CMAKE_SOURCE_DIR}/*.cpp,*.h,*.cu,*.cxx,*.cppm ${ASTYLE_OPTIONS})
