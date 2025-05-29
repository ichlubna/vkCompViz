cmake_minimum_required(VERSION 4.0)
project(simpleBlending)

add_executable(${PROJECT_NAME} simpleBlending.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE vkCompViz)
target_sources(${PROJECT_NAME}
    PUBLIC FILE_SET CXX_MODULES FILES
    external/arguments/arguments.cxx
)
target_compile_features(${PROJECT_NAME}
    PRIVATE cxx_std_26
)
target_compile_options(${PROJECT_NAME}
    PRIVATE 
    -Wall
    -Wextra
    -pedantic
)
