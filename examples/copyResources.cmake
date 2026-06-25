function(copyResources INPUT_TARGET INPUT_RESOURCES)
    set(RESOURCES ${INPUT_RESOURCES}
        vkCompViz/shaders/fullScreenVS.slang
        vkCompViz/shaders/textureDisplayFS.slang
        vkCompViz/shaders/vertexFragmentCommons.slang
    )

    set(OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

    add_custom_target(copy_${INPUT_TARGET}_resources ALL)

    foreach(RESOURCE_FILE IN LISTS RESOURCES)
        get_filename_component(FILE_NAME "${RESOURCE_FILE}" NAME)

        set(IN_FILE "${CMAKE_SOURCE_DIR}/${RESOURCE_FILE}")
        set(OUT_FILE "${OUT_DIR}/${FILE_NAME}")

        add_custom_command(
            TARGET copy_${INPUT_TARGET}_resources
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying ${RESOURCE_FILE}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy
                    "${IN_FILE}"
                    "${OUT_FILE}"
            VERBATIM
        )
    endforeach()

    add_dependencies(${INPUT_TARGET} copy_${INPUT_TARGET}_resources)
endfunction()
