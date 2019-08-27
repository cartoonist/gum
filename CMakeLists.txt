cmake_minimum_required(VERSION 3.8)
project(gum VERSION 0.0.1 LANGUAGES CXX)

# Adding CMake module path.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
# Finding dependencies.
find_package(SparseHash REQUIRED)
find_package(GFAKluge)
find_package(VGio)
find_package(OpenMP)

# Adding test submodule.
add_subdirectory(test)

# Registering tests.
include(CTest)
add_test(NAME TestUtils COMMAND Tests "[utils]")
add_test(NAME TestSeqGraph COMMAND Tests "[seqgraph]")
