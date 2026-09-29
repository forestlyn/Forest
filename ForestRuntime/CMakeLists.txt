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

# Headless UI data/layout tests; no renderer, window or Mono initialization required.
add_executable(UILayoutTests EXCLUDE_FROM_ALL tests/UILayoutTests.cpp)
target_link_libraries(UILayoutTests PRIVATE Engine)
set_target_properties(UILayoutTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/runtime/$<CONFIG>")

# Offscreen pixel integration tests use a hidden OpenGL window and the real render thread.
add_executable(UIRenderTests EXCLUDE_FROM_ALL tests/UIRenderTests.cpp src/RuntimeLayer.cpp)
target_link_libraries(UIRenderTests PRIVATE Engine)
target_include_directories(UIRenderTests PRIVATE "${CMAKE_SOURCE_DIR}/Engine/ThirdParty/glad/include")
set_target_properties(UIRenderTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/runtime/$<CONFIG>")
add_dependencies(UIRenderTests ForestRuntimeResources)
