# Engine动态库配置
project(Engine)

file(GLOB ENGINE_SOURCES "src/Engine/*.cpp" "src/Engine/**/*.cpp" "src/Platform/**/*.cpp")
file(GLOB ENGINE_HEADERS "src/Engine/*.h" "src/Engine/**/*.h" "src/Platform/**/*.h")


add_subdirectory("ThirdParty/glfw")
add_subdirectory("ThirdParty/glad")
add_subdirectory("ThirdParty/imgui")

# 创建动态库
add_library(Engine SHARED ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# 设置包含目录
target_include_directories(Engine
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glad/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Engine PUBLIC glfw glad opengl32 imgui)

# 设置编译器特性
target_compile_features(Engine PUBLIC cxx_std_17)

# 设置预处理器定义
target_compile_definitions(Engine
    PRIVATE FOREST_BUILD_DLL
    PUBLIC FOREST_PLATFORM_WINDOWS
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