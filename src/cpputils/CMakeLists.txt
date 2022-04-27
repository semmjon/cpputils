cmake_minimum_required(VERSION 3.4...3.18)
set(CMAKE_CXX_STANDARD 17)

include_directories(../../lib)
file(GLOB_RECURSE HASH_LIB_SOURCE_FILES ../../lib/hash-library/*.cpp)
list(FILTER HASH_LIB_SOURCE_FILES EXCLUDE REGEX ".*test.*")
file(GLOB_RECURSE SOURCE_FILES *.cpp)
pybind11_add_module(cpputils ${SOURCE_FILES} ${HASH_LIB_SOURCE_FILES})

target_include_directories(cpputils PUBLIC ../../lib)
set_target_properties(cpputils PROPERTIES OUTPUT_NAME cpputils/_cpputils)
target_compile_definitions(cpputils
                           PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})

add_compile_options("-Wall" "-Wpedantic" "-Wextra" "-fexceptions"
                    "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>")
