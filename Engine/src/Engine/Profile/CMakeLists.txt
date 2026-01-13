set(PROFILING_CONFIG "${CMAKE_SOURCE_DIR}/profiling.cfg")
message(STATUS "Using profiling config: ${PROFILING_CONFIG}")
if(NOT EXISTS ${PROFILING_CONFIG})
    message(WARNING "profiling.cfg not found, use global profiling off")
    set(DEFAULT_PROFILING 0)
else()
    # 2. 解析配置文件为键值对（格式：FILE_PATTERN=ENABLE）
    file(STRINGS ${PROFILING_CONFIG} PROFILING_LINES)
    set(PROFILING_RULES "")
    foreach(LINE ${PROFILING_LINES})
        # 跳过注释和空行
        string(STRIP ${LINE} LINE)
        if(LINE STREQUAL "" OR LINE MATCHES "^#")
            continue()
        endif()
        # 分割键值对（按=分割）
        string(REPLACE "=" ";" KEY_VAL ${LINE})
        list(GET KEY_VAL 0 FILE_PATTERN)
        list(GET KEY_VAL 1 ENABLE)
        # 存储规则（格式："PATTERN:ENABLE"）
        list(APPEND PROFILING_RULES "${FILE_PATTERN}:${ENABLE}")
    endforeach()
    # 3. 默认全局开关（无匹配规则时使用）
    set(DEFAULT_PROFILING 0)
endif()
# 4. 为每个源文件设置预处理器定义
foreach(SRC_FILE ${SOURCE_FILES})
    # 获取文件的绝对路径（统一匹配规则）
    get_filename_component(SRC_ABS_PATH ${SRC_FILE} ABSOLUTE)
    file(TO_CMAKE_PATH "${SRC_ABS_PATH}" SRC_ABS_PATH)
    
    # 默认启用状态（继承全局）
    set(FILE_PROFILING ${DEFAULT_PROFILING})
    # 匹配profiling.cfg中的规则
    foreach(RULE ${PROFILING_RULES})
        string(REPLACE ":" ";" RULE_PARTS ${RULE})
        list(GET RULE_PARTS 0 PATTERN)
        list(GET RULE_PARTS 1 ENABLE)
        # 转换为CMake的通配符匹配（支持*）
        file(GLOB MATCHED_FILES "${CMAKE_SOURCE_DIR}/${PATTERN}")
        if("${SRC_ABS_PATH}" IN_LIST MATCHED_FILES)
            set(FILE_PROFILING ${ENABLE})
            break() # 匹配到第一条规则即停止
        endif()
    endforeach()
    # 为当前文件设置编译宏：FOREST_ENABLE_PROFILING_FILE
    # message(STATUS "Profiling config for ${SRC_FILE}: ${FILE_PROFILING}")
    set_source_files_properties(
        ${SRC_FILE}
        PROPERTIES
        COMPILE_DEFINITIONS "FOREST_ENABLE_PROFILING_FILE=${FILE_PROFILING}"
    )
endforeach()