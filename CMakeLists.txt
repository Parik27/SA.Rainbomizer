# rainbomizer
cmake_minimum_required(VERSION 3.1)
project(rainbomizer)

# Sources
include_directories(include)
file(GLOB SOURCES src/*.cc)

add_library("rainbomizer" SHARED ${SOURCES})

# Properties
set_target_properties("rainbomizer" PROPERTIES SUFFIX ".asi")
target_compile_features("rainbomizer" PRIVATE cxx_auto_type)
