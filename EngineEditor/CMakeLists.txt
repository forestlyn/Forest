# EngineEditor应用程序配置
project(EngineEditor)

# 设置源文件
file(GLOB_RECURSE ENGINEEDITOR_SOURCES "src/*.cpp")
# 设置头文件目录
file(GLOB_RECURSE ENGINEEDITOR_HEADERS "src/*.h")

# Configure Profiling
set(SOURCE_FILES ${ENGINEEDITOR_SOURCES})
include("${CMAKE_SOURCE_DIR}/Engine/src/Engine/Profile/CMakeLists.txt")

# 创建可执行文件
add_executable(EngineEditor ${ENGINEEDITOR_SOURCES} ${ENGINEEDITOR_HEADERS})
# 设置包含目录
target_include_directories(EngineEditor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/src/ThirdParty/spdlog/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/ThirdParty/ImGuizmo
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 链接Engine库
target_link_libraries(EngineEditor PRIVATE Engine)
# 设置可执行文件的输出名称
set_target_properties(EngineEditor PROPERTIES
    OUTPUT_NAME "EngineEditor"
    DEBUG_POSTFIX "_debug"
    RELEASE_POSTFIX "_release"
)

# 复制运行时依赖（如果需要）
# 在Windows上可能需要复制DLL文件
if(WIN32)
    add_custom_command(TARGET EngineEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Engine>
            $<TARGET_FILE_DIR:EngineEditor>
    )
endif()

# 拷贝 assets 到可执行文件目录（在构建后执行）
# if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
#     add_custom_command(TARGET EngineEditor POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_directory
#             "${CMAKE_CURRENT_SOURCE_DIR}/resources/assets"
#             "$<TARGET_FILE_DIR:EngineEditor>/assets"
#         COMMENT "Copying assets to runtime folder"
#     )
#     add_custom_command(TARGET EngineEditor POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#             "${CMAKE_CURRENT_SOURCE_DIR}/resources/imgui.ini"
#             "$<TARGET_FILE_DIR:EngineEditor>/"
#         COMMENT "Copying imgui.ini to runtime folder")
# endif()

# 安装配置
install(TARGETS EngineEditor
    RUNTIME DESTINATION bin
)