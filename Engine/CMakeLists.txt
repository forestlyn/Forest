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
list(APPEND ENGINE_HEADERS "ThirdParty/filewatch/FileWatch.hpp")

add_subdirectory("ThirdParty/glfw")
add_subdirectory("ThirdParty/glad")
add_subdirectory("ThirdParty/imgui")
add_subdirectory("ThirdParty/yaml-cpp")
add_subdirectory("ThirdParty/Box2D")

# find vulkan sdk library
set(SHADERC_ROOT $ENV{VULKAN_SDK})
message("Current Vulkan SDK path: ${SHADERC_ROOT}")
set(SHADERC_INCLUDE_DIR "${SHADERC_ROOT}/Include")
message("SHADERC_INCLUDE_DIR set to: ${SHADERC_INCLUDE_DIR}")
set(SHADERC_LIB_DIR "${SHADERC_ROOT}/Lib")
message("SHADERC_LIB_DIR set to: ${SHADERC_LIB_DIR}")

# Select prebuilt libraries at build time for multi-config generators.
set(SHADERC_LIBRARYS)
foreach(library shaderc_combined shaderc_shared spirv-cross-core spirv-cross-glsl SPIRV-Tools)
    list(APPEND SHADERC_LIBRARYS "${SHADERC_LIB_DIR}/${library}$<$<CONFIG:Debug>:d>.lib")
endforeach()

set(MONO_LIBRARIES)
file(GLOB mono_library_names RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mono/lib/Debug"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mono/lib/Debug/*.lib")
foreach(library IN LISTS mono_library_names)
    list(APPEND MONO_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mono/lib/$<IF:$<CONFIG:Debug>,Debug,Release>/${library}")
endforeach()

# mono dependency
set(MONO_DEPENDENCIES Ws2_32 Winmm Version Bcrypt)

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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Box2D/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mono/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/filewatch>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glad/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/stb_image>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGuizmo>
        $<BUILD_INTERFACE:${SHADERC_INCLUDE_DIR}>
)

target_link_libraries(Engine PUBLIC glfw glad opengl32 imgui yaml-cpp box2d)
target_link_libraries(Engine PRIVATE ${SHADERC_LIBRARYS} ${MONO_LIBRARIES} ${MONO_DEPENDENCIES})
# 设置编译器特性
target_compile_features(Engine PUBLIC cxx_std_20)

# 设置预处理器定义
target_compile_definitions(Engine
    PRIVATE FOREST_BUILD_DLL
        FOREST_CMAKE_EXECUTABLE="${CMAKE_COMMAND}"
        FOREST_SCRIPT_BUILD_DRIVER="${CMAKE_SOURCE_DIR}/Scripts/ProjectScripts"
        FOREST_RUNTIME_RELEASE_DIR="${CMAKE_BINARY_DIR}/runtime/Release"
    PUBLIC FOREST_PLATFORM_WINDOWS
    FOREST_ENABLE_ASSERTS
    GLFW_INCLUDE_NONE
    FOREST_ENABLE_PROFILING_GLOBAL
    ENGINE_ENABLE_PROFILELAYER
    ENGINE_ENABLE_RENDERTHREAD
)

target_precompile_headers(
    Engine PRIVATE "src/Engine/pcheader.h"
)

# 设置库的输出名称
set_target_properties(Engine PROPERTIES
    OUTPUT_NAME "Engine"
    DEBUG_POSTFIX "_debug"
    RELEASE_POSTFIX "_release"
)

# 安装配置
install(TARGETS Engine
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)
