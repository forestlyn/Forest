project(glad)

file(GLOB GLAD_SOURCES "src/*.c")
file(GLOB GLAD_HEADERS "include/glad/*.h" "include/KHR/*.h")

add_library(glad STATIC ${GLAD_SOURCES} ${GLAD_HEADERS})

target_include_directories(glad
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(glad PUBLIC cxx_std_17)

# 设置库的输出名称

set_target_properties(glad PROPERTIES
    OUTPUT_NAME "glad"
)

# 安装配置

install(TARGETS glad
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)

