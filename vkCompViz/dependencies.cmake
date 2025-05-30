set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

find_package(FFMPEG REQUIRED)
add_library(ffmpeg INTERFACE)
target_link_libraries(ffmpeg INTERFACE ${FFMPEG_LIBRARIES})
target_include_directories(ffmpeg INTERFACE $<BUILD_INTERFACE:${FFMPEG_INCLUDE_DIRS})

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_library( VulkanHppModule )
target_sources( VulkanHppModule PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS ${Vulkan_INCLUDE_DIR}
  FILES ${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm
)
target_compile_features( VulkanHppModule PUBLIC cxx_std_26 )
target_link_libraries( VulkanHppModule PUBLIC Vulkan::Vulkan)
