cmake_minimum_required(VERSION 4.0)
project(3DViewer)
include(../copyResources.cmake)

add_executable(${PROJECT_NAME} 3DViewer.cpp objLoader.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE vkCompViz)
target_sources(${PROJECT_NAME}
    PUBLIC FILE_SET CXX_MODULES FILES
    objLoader.cppm
)

set(RESOURCE_FILES
    examples/${PROJECT_NAME}/shaders/3DViewerVertex.slang
    examples/${PROJECT_NAME}/shaders/3DViewerFragment.slang
    examples/${PROJECT_NAME}/shaders/3DViewerCompute.slang
    examples/${PROJECT_NAME}/shaders/inputs.slang
)
copyResources()
target_compile_features(${PROJECT_NAME}
    PRIVATE cxx_std_26
)
target_compile_options(${PROJECT_NAME}
    PRIVATE 
    -Wall
    -Wextra
    -pedantic
)
