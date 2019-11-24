# rainbomizer
cmake_minimum_required(VERSION 3.1)
project(rainbomizer)

# C++20
if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

# Sources
include_directories(include)
file(GLOB SOURCES src/*.cc)

add_library("rainbomizer" SHARED ${SOURCES})

# Properties
set_target_properties("rainbomizer" PROPERTIES SUFFIX ".asi")
target_compile_features("rainbomizer" PRIVATE cxx_auto_type)
