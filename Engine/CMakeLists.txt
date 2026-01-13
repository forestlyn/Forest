project(Engine)

file(GLOB_RECURSE ENGINE_SOURCES 
    "src/Engine/*.cpp" 
    "src/Platform/*.cpp"
)
list(APPEND ENGINE_SOURCES "ThirdParty/stb_image/stb_image.cpp")
list(APPEND ENGINE_SOURCES "ThirdParty/ImGuizmo/ImGuizmo.cpp")

file(GLOB_RECURSE ENGINE_HEADERS 
    "src/Engine/*.h" 
    "src/Platform/*.h"
)
list(APPEND ENGINE_HEADERS "ThirdParty/stb_image/stb_image.h")
list(APPEND ENGINE_HEADERS "ThirdParty/ImGuizmo/ImGuizmo.h")

add_subdirectory("ThirdParty/glfw")
add_subdirectory("ThirdParty/glad")
add_subdirectory("ThirdParty/imgui")
add_subdirectory("ThirdParty/yaml-cpp")

# 设置 shaderc 路径
set(SHADERC_ROOT $ENV{VULKAN_SDK})
message("Current Vulkan SDK path: ${SHADERC_ROOT}")
set(SHADERC_INCLUDE_DIR "${SHADERC_ROOT}/Include")
message("SHADERC_INCLUDE_DIR set to: ${SHADERC_INCLUDE_DIR}")
set(SHADERC_LIB_DIR "${SHADERC_ROOT}/Lib")
message("SHADERC_LIB_DIR set to: ${SHADERC_LIB_DIR}")

find_library(SHADERC_LIBRARY NAMES shaderc_combined
    PATHS ${SHADERC_LIB_DIR}
    DOC "shaderc library")

if (SHADERC_INCLUDE_DIR AND SHADERC_LIBRARY)
    message(STATUS "Found shaderc: ${SHADERC_LIBRARY}")
    message(STATUS "shaderc include dir: ${SHADERC_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "shaderc not found!")
endif()

add_library(shaderc_combined STATIC IMPORTED)
set_target_properties(shaderc_combined PROPERTIES
    IMPORTED_LOCATION "${SHADERC_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SHADERC_INCLUDE_DIR}"
)

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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/entt/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/yaml-cpp/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glad/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/stb_image>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGuizmo>
        $<BUILD_INTERFACE:${SHADERC_INCLUDE_DIR}>
)

target_link_libraries(Engine PUBLIC glfw glad opengl32 imgui yaml-cpp)
target_link_libraries(Engine PRIVATE shaderc_combined)
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