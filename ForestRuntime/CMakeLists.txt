add_executable(ForestRuntime src/RuntimeApp.cpp src/RuntimeLayer.cpp)
target_link_libraries(ForestRuntime PRIVATE Engine)
target_compile_definitions(ForestRuntime PRIVATE FOREST_SOURCE_ROOT="${CMAKE_SOURCE_DIR}")
set_target_properties(ForestRuntime PROPERTIES DEBUG_POSTFIX "_debug" RELEASE_POSTFIX "_release")
# Development runner: use the repository's resources and Mono installation.
add_dependencies(ForestRuntime ScriptCore)
add_custom_command(TARGET ForestRuntime POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/Engine/ThirdParty/mono/bin/$<CONFIG>/mono-2.0-sgen.dll"
        "$<TARGET_FILE_DIR:ForestRuntime>")
