# Resource staging runs on every build, including when only an asset changed.
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
set(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
include(InstallRequiredSystemLibraries)
get_filename_component(FOREST_MSVC_TOOL_DIRECTORY "${CMAKE_LINKER}" DIRECTORY)
configure_file("${CMAKE_SOURCE_DIR}/Scripts/PrepareRuntime.cmake.in"
               "${CMAKE_BINARY_DIR}/PrepareRuntime.cmake" @ONLY)

function(forest_runtime_resources target editor)
    add_custom_target(${target}Resources
        COMMAND ${CMAKE_COMMAND}
            "-DDESTINATION=$<TARGET_FILE_DIR:${target}>"
            "-DCORE_ASSEMBLY=$<TARGET_FILE:ScriptCore>"
            "-DCONFIG=$<CONFIG>" "-DEDITOR=${editor}"
            -P "${CMAKE_BINARY_DIR}/PrepareRuntime.cmake"
        DEPENDS ScriptCore VERBATIM)
    add_dependencies(${target} ${target}Resources)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            "-DDESTINATION=$<TARGET_FILE_DIR:${target}>"
            "-DEXECUTABLE=$<TARGET_FILE:${target}>"
            "-DCONFIG=$<CONFIG>" -DDEPENDENCIES_ONLY=ON
            -P "${CMAKE_BINARY_DIR}/PrepareRuntime.cmake"
        VERBATIM)
endfunction()
