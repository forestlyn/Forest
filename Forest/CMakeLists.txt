# Forest应用程序配置
project(Forest)

# 设置源文件
file(GLOB FOREST_SOURCES "src/*.cpp" "src/**/*.cpp")

# 设置头文件目录
file(GLOB FOREST_HEADERS "src/*.h" "src/**/*.h")

# 创建可执行文件
add_executable(Forest ${FOREST_SOURCES} ${FOREST_HEADERS})

# 设置包含目录
target_include_directories(Forest
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/src/ThirdParty/spdlog/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 链接Engine库
target_link_libraries(Forest PRIVATE Engine)
# 设置可执行文件的输出名称
set_target_properties(Forest PROPERTIES
    OUTPUT_NAME "Forest"
    DEBUG_POSTFIX "-debug"
)

# 复制运行时依赖（如果需要）
# 在Windows上可能需要复制DLL文件
if(WIN32)
    add_custom_command(TARGET Forest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Engine>
            $<TARGET_FILE_DIR:Forest>
    )
endif()

# 拷贝 assets 到可执行文件目录（在构建后执行）
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    add_custom_command(TARGET Forest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:Forest>/assets"
        COMMENT "Copying assets to runtime folder"
    )
endif()

# 安装配置
install(TARGETS Forest
    RUNTIME DESTINATION bin
)