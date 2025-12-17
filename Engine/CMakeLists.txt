project(Engine)

file(GLOB_RECURSE ENGINE_SOURCES 
    "src/Engine/*.cpp" 
    "src/Platform/*.cpp"
)
list(APPEND ENGINE_SOURCES "ThirdParty/stb_image/stb_image.cpp")

file(GLOB_RECURSE ENGINE_HEADERS 
    "src/Engine/*.h" 
    "src/Platform/*.h"
)
list(APPEND ENGINE_HEADERS "ThirdParty/stb_image/stb_image.h")

add_subdirectory("ThirdParty/glfw")
add_subdirectory("ThirdParty/glad")
add_subdirectory("ThirdParty/imgui")

# Configure Profiling
set(SOURCE_FILES ${ENGINE_SOURCES})
include("src/Engine/Profile/CMakeLists.txt")

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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/stb_image>
)

target_link_libraries(Engine PUBLIC glfw glad opengl32 imgui)

# 设置编译器特性
target_compile_features(Engine PUBLIC cxx_std_20)

# 设置预处理器定义
target_compile_definitions(Engine
    PRIVATE FOREST_BUILD_DLL
    PUBLIC FOREST_PLATFORM_WINDOWS
    FOREST_ENABLE_ASSERTS
    GLFW_INCLUDE_NONE
    FOREST_ENABLE_PROFILING_GLOBAL
    ENGINE_ENABLE_PROFILELAYER
)

target_precompile_headers(
    Engine PRIVATE "src/Engine/pcheader.h"
)

# 设置库的输出名称
set_target_properties(Engine PROPERTIES
    OUTPUT_NAME "Engine"
    DEBUG_POSTFIX "-debug"
    RELEASE_POSTFIX "_release"
)

# 安装配置
install(TARGETS Engine
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)