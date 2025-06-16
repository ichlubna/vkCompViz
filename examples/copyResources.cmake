function(copyResources)
    set(RESOURCE_FILES ${RESOURCE_FILES} 
        vkCompViz/shaders/fullScreenVS.slang
        vkCompViz/shaders/splitScreenFS.slang
        vkCompViz/shaders/textureDisplayFS.slang
    )

    foreach(RESOURCE_FILE IN LISTS RESOURCE_FILES)
        get_filename_component(FILE_NAME "${RESOURCE_FILE}" NAME)
        set(IN_FILE "${CMAKE_SOURCE_DIR}/${RESOURCE_FILE}")
        set(OUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/shaders/${FILE_NAME}")
        message(STATUS "Copying ${IN_FILE} to ${OUT_FILE}")
        add_custom_command(
            TARGET ${PROJECT_NAME} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${IN_FILE}"
                    "${OUT_FILE}")
    endforeach()
endfunction()
