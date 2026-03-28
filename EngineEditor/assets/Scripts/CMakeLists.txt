project(Sandbox CSharp)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "src/*.cs")

add_library(Sandbox SHARED ${SOURCE_FILES})

set(SCRIPT_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/EngineEditor/resources/scripts")

target_link_libraries(Sandbox PRIVATE ScriptCore)

# 添加调试符号配置
set_property(TARGET Sandbox PROPERTY VS_GLOBAL_DebugSymbols true)
set_property(TARGET Sandbox PROPERTY VS_GLOBAL_DebugType portable)
set_property(TARGET Sandbox PROPERTY VS_GLOBAL_Optimize false)

# 确保 Debug 配置生成 PDB 文件
set_property(TARGET Sandbox APPEND PROPERTY COMPILE_FLAGS_DEBUG "/debug:portable /optimize-")

set_target_properties(Sandbox PROPERTIES
    VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2"
    OUTPUT_NAME "Sandbox"
    RUNTIME_OUTPUT_DIRECTORY "${SCRIPT_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${SCRIPT_OUTPUT_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${SCRIPT_OUTPUT_DIR}/bin"
    # 确保 PDB 文件与 DLL 在同一目录
    PDBS_OUTPUT_DIRECTORY_DEBUG "${SCRIPT_OUTPUT_DIR}/bin"
    PDBS_OUTPUT_DIRECTORY_RELEASE "${SCRIPT_OUTPUT_DIR}/bin"
)