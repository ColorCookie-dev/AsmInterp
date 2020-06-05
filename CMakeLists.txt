cmake_minimum_required(VERSION 3.5)

project(AsmInterp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(src_dir "${CMAKE_SOURCE_DIR}/src")

add_compile_options("-Wall" "-Wpedantic" "-Wextra" "-O3")

file(GLOB src_files "${src_dir}/*.cpp")

add_executable(AsmInterp "${src_files}")
