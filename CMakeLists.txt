cmake_minimum_required(VERSION 4.0)
project(vkCompViz)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

set(CORE_SOURCES 
src/app.cpp
examples/simpleBlending.cpp
)

function(addExample NAME SOURCES DESCRIPTION LIBRARIES FEATURES OPTIONS)
    option(${NAME} ${DESCRIPTION} 1)
    if(${NAME})
        set(EXAMPLE_SOURCES 
        ${CORE_SOURCES}
        ${SOURCES}
        )
        add_executable(${NAME} ${EXAMPLE_SOURCES})
        target_link_libraries(${NAME} ${LIBRARIES})
        target_compile_features(${NAME} PRIVATE ${FEATURES})
        target_compile_options(${NAME} PRIVATE ${OPTIONS})
    endif()
endFunction()

set(COMMON_OPTIONS 
-Wall
-Wextra
-pedantic
)

set(COMMON_FEATURES 
cxx_std_23
)

set(COMMON_LIBRARIES
vulkan
glfw
)

addExample(SimpleBlending examples/simpleBlending.cpp "Hello world example that blends two images" "${COMMON_LIBRARIES}" "${COMMON_FEATURES}" "${COMMON_OPTIONS}")

set(ASTYLE_BINARY "astyle" CACHE PATH "Path to astyle binary to format the code by \"make format\"")
set(ASTYLE_OPTIONS -A1 -C -n -k3 -p -U -xg -r CACHE STRING "Astyle options used by \"make format\"")
add_custom_target(format WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
add_custom_command(TARGET format PRE_BUILD COMMAND ${ASTYLE_BINARY} ${CMAKE_SOURCE_DIR}/*.cpp,*.h,*.cu,*.cxx,*.cppm ${ASTYLE_OPTIONS})
