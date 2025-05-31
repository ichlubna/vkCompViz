cmake_minimum_required(VERSION 4.0)
project(vkCompViz)
include(dependencies.cmake)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME}
    PUBLIC
    src/vkCompViz.app.cpp
    src/gpuVulkan.cpp
    src/windowGlfw.cpp
    src/loaderImageFfmpeg.cpp
)
target_sources(${PROJECT_NAME}
    PUBLIC FILE_SET CXX_MODULES FILES
    include/vkCompViz.cppm
    src/vkCompViz.app.cppm
    src/gpu.cppm
    src/gpuVulkan.cppm
    src/window.cppm
    src/windowGlfw.cppm
    src/loader.cppm
    src/loaderImageFfmpeg.cppm
)
target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(${PROJECT_NAME}
    VulkanHppModule
    glfw
    glm
    ffmpeg swscale avfilter ${FFMPEG_LIBRARIES} 
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

