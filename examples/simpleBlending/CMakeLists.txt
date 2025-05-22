cmake_minimum_required(VERSION 4.0)
project(simpleBlending)

add_executable(${PROJECT_NAME} simpleBlending.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE vkCompViz)
target_compile_features(${PROJECT_NAME}
    PRIVATE cxx_std_23
)
target_compile_options(${PROJECT_NAME}
    PRIVATE 
    -Wall
    -Wextra
    -pedantic
)
