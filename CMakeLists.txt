cmake_minimum_required(VERSION 3.15)
project(Forest)
if (MSVC)
    # 设置源码字符集为 UTF-8
    add_compile_options(/source-charset:utf-8)
    # 设置执行字符集为 UTF-8（程序运行时的字符编码）
    add_compile_options(/execution-charset:utf-8)
    # 可选：禁用默认的 Unicode 宏（避免和 UTF-8 冲突）
    add_definitions(-DUNICODE -D_UNICODE)
endif()
# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(BUILD_BIN_DIR "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Output directory for executables")
set(BUILD_LIB_DIR "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Output directory for libraries")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_LIB_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_BIN_DIR})

# 配置类型设置
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES Debug Release)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# 添加子目录
add_subdirectory(Engine)
add_subdirectory(EngineEditor)
add_subdirectory(Forest)
