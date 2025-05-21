cmake_minimum_required(VERSION 4.0)
project(vkCompViz)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME}
    PUBLIC
    src/vkCompViz.app.cpp
)
target_sources(${PROJECT_NAME}
    PUBLIC FILE_SET CXX_MODULES FILES
    include/vkCompViz.cppm
    src/vkCompViz.app.cppm
)
target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(${PROJECT_NAME}
    vulkan
    glfw
)
target_compile_features(${PROJECT_NAME}
    PRIVATE cxx_std_23
)
target_compile_options(${PROJECT_NAME}
    PRIVATE 
    -Wall
    -Wextra
    -pedantic
)

