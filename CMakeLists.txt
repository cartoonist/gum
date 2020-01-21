cmake_minimum_required(VERSION 3.8)
project(gum VERSION 0.0.1 LANGUAGES CXX)

# Adding CMake module path.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
# Finding dependencies.
find_package(SparseHash REQUIRED)
find_package(SDSL REQUIRED)
find_package(GFAKluge)
find_package(VGio)
find_package(OpenMP)
# Include external dependencies
include(${PROJECT_SOURCE_DIR}/ext/external.cmake)

# Force CMake to use `-I` instead of `-isystem`
set(CMAKE_NO_SYSTEM_FROM_IMPORTED TRUE)

# Setting header files
file(GLOB HEADER_FILES "${PROJECT_SOURCE_DIR}/include/gum/*.hpp")
# Creating an INTERFACE library
add_library(gum INTERFACE)
# Defining target sources
target_sources(gum INTERFACE "$<BUILD_INTERFACE:${HEADER_FILES}>")
# Defining include directories
target_include_directories(gum
  INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/gum>
  SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/gum>
  INTERFACE sdsl::sdsl
  INTERFACE SparseHash::SparseHash
  INTERFACE GFAKluge::GFAKluge
  INTERFACE VGio::VGio)
# Defining link libraries
target_link_libraries(gum
  INTERFACE sdsl::sdsl
  INTERFACE VGio::VGio
  INTERFACE GFAKluge::GFAKluge
  INTERFACE OpenMP::OpenMP_CXX)
# Use C++17
target_compile_features(gum INTERFACE cxx_std_17)
# Defining `gum::gum` alias
add_library(gum::gum ALIAS gum)
# Install targets
install(DIRECTORY include/gum DESTINATION include)

# Adding test submodule.
add_subdirectory(test)

# Registering tests.
include(CTest)
add_test(NAME TestUtils COMMAND Tests "[utils]")
add_test(NAME TestSeqGraph COMMAND Tests "[seqgraph]")
add_test(NAME TestAlphabet COMMAND Tests "[alphabet]")
add_test(NAME TestStringSet COMMAND Tests "[stringset]")
