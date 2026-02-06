project(Sandbox CSharp)

file(GLOB_RECURSE SOURCE_FILES "src/*.cs")

add_library(Sandbox SHARED ${SOURCE_FILES})

set(SCRIPT_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/EngineEditor/resources/scripts")

target_link_libraries(Sandbox PRIVATE ScriptCore)

set_target_properties(Sandbox PROPERTIES
    VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2"
    OUTPUT_NAME "Sandbox"
    RUNTIME_OUTPUT_DIRECTORY "${SCRIPT_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${SCRIPT_OUTPUT_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${SCRIPT_OUTPUT_DIR}/bin"
)