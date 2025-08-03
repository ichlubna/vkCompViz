cmake_minimum_required(VERSION 4.0)
project(vkCompViz)
include(dependencies.cmake)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME}
    PUBLIC
    src/vkCompViz.app.cpp
    src/gpu.interface.cpp
    src/gpu.vulkan.cpp
    src/window.glfw.cpp
    src/loader.imageFfmpeg.cpp
    src/shader.slang.cpp
    src/parameterParser.cpp
)
target_sources(${PROJECT_NAME}
    PUBLIC FILE_SET CXX_MODULES FILES
    include/vkCompViz.cppm
    src/common.cppm
    src/vkCompViz.app.cppm
    src/parameterParser.cppm
    src/timer.cppm
    src/gpu.cppm
    src/gpu.interface.cppm
    src/gpu.vulkan.cppm
    src/window.cppm
    src/window.interface.cppm
    src/glfwKeyStrings.cppm
    src/window.glfw.cppm
    src/loader.cppm
    src/loader.interface.cppm
    src/loader.imageFfmpeg.cppm
    src/shader.cppm
    src/shader.interface.cppm
    src/shader.slang.cppm
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
    GPUOpen::VulkanMemoryAllocator
    glfw
    slang::slang
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

