cmake_minimum_required(VERSION 3.15)
project(ForestProjectScripts LANGUAGES CSharp)
foreach(required FOREST_SCRIPT_SOURCE FOREST_SCRIPT_ASSEMBLY FOREST_SCRIPT_CORE)
    if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
        message(FATAL_ERROR "${required} is required")
    endif()
    file(TO_CMAKE_PATH "${${required}}" ${required})
endforeach()
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${FOREST_SCRIPT_SOURCE}/*.cs")
if(NOT sources)
    message(FATAL_ERROR "No C# sources found in ${FOREST_SCRIPT_SOURCE}")
endif()
get_filename_component(output_dir "${FOREST_SCRIPT_ASSEMBLY}" DIRECTORY)
get_filename_component(assembly_name "${FOREST_SCRIPT_ASSEMBLY}" NAME_WE)
add_library(GameScripts SHARED ${sources})
set_target_properties(GameScripts PROPERTIES
    VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2"
    VS_DOTNET_REFERENCES "System;System.Core;System.Numerics"
    VS_DOTNET_REFERENCE_Engine-ScriptCore "${FOREST_SCRIPT_CORE}"
    OUTPUT_NAME "${assembly_name}"
    RUNTIME_OUTPUT_DIRECTORY "${output_dir}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${output_dir}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${output_dir}")
