project(Engine)

file(GLOB ENGINE_SOURCES "src/Engine/*.cpp" "src/Engine/**/*.cpp" "src/Platform/**/*.cpp")
file(GLOB ENGINE_HEADERS "src/Engine/*.h" "src/Engine/**/*.h" "src/Platform/**/*.h")


add_subdirectory("ThirdParty/glfw")
add_subdirectory("ThirdParty/glad")
add_subdirectory("ThirdParty/imgui")
add_subdirectory("ThirdParty/DirectX-Headers")

add_library(Engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# 设置包含目录
target_include_directories(Engine
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glad/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/DirectX-Headers/include>
)

target_link_libraries(Engine PUBLIC glfw glad opengl32 imgui)


target_link_libraries(${PROJECT_NAME} PRIVATE
    d3d12.lib        # 核心库（包含辅助类的实现）
    dxgi.lib         # DXGI 交换链库
    d3dcompiler.lib  # HLSL 着色器编译库
    user32.lib       # Windows 窗口基础库
    gdi32.lib        # 图形设备接口库
    shell32.lib      # 系统外壳库
)


# 设置编译器特性
target_compile_features(Engine PUBLIC cxx_std_17)

# 设置预处理器定义
target_compile_definitions(Engine
    PRIVATE FOREST_BUILD_DLL
    PUBLIC FOREST_PLATFORM_WINDOWS
    # PUBLIC FOREST_PLATFORM_OPENGL
    PUBLIC FOREST_PLATFORM_DIRECTX12
    FOREST_ENABLE_ASSERTS
    GLFW_INCLUDE_NONE
)

target_precompile_headers(
    Engine PRIVATE "src/Engine/pcheader.h"
)

# 设置库的输出名称
set_target_properties(Engine PROPERTIES
    OUTPUT_NAME "Engine"
    DEBUG_POSTFIX "-debug"
)

# 安装配置
install(TARGETS Engine
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)