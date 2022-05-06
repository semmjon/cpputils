cmake_minimum_required(VERSION 3.4...3.22)
project(cpputils)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/pybind11)
include_directories(lib)
add_subdirectory(src/cpputils)

add_compile_options(
  "-Wall"
  "-Wpedantic"
  "-Wextra"
  "-fexceptions"
  "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
  "-Wall"
  "-Wextra"
  "-Wundef"
  "-pedantic"
  "-msse2"
  "-msse3"
  "-mpopcnt"
  "-funsafe-math-optimizations"
  "-fno-math-errno")
