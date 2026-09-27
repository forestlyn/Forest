add_executable(ForestRuntime src/RuntimeApp.cpp src/RuntimeLayer.cpp)
target_link_libraries(ForestRuntime PRIVATE Engine)
set_target_properties(ForestRuntime PROPERTIES
    DEBUG_POSTFIX "_debug" RELEASE_POSTFIX "_release"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/runtime/$<CONFIG>")
forest_runtime_resources(ForestRuntime FALSE)

# Explicit integration target; not part of normal game/editor builds.
add_executable(ProjectScriptLifecycleTests EXCLUDE_FROM_ALL tests/ProjectScriptLifecycleTests.cpp)
target_link_libraries(ProjectScriptLifecycleTests PRIVATE Engine)
set_target_properties(ProjectScriptLifecycleTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/runtime/$<CONFIG>")
add_dependencies(ProjectScriptLifecycleTests ForestRuntimeResources)
add_dependencies(ProjectScriptLifecycleTests ScriptCore)

add_executable(ResourcePathTests EXCLUDE_FROM_ALL tests/ResourcePathTests.cpp)
target_link_libraries(ResourcePathTests PRIVATE Engine)
set_target_properties(ResourcePathTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/runtime/$<CONFIG>")
