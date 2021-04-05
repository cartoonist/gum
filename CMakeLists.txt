cmake_minimum_required(VERSION 3.10)
project(gum VERSION 0.0.1 LANGUAGES CXX)

# Options
option(BUILD_TESTING "Build test programs" OFF)
option(BUILD_AUX_TOOLS "Build auxiliary tools" OFF)

# Adding CMake module path.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
# Finding dependencies.
find_package(ZLIB REQUIRED)
find_package(sdsl REQUIRED)
find_package(ParallelHashmap)
find_package(GFAKluge)
find_package(libbdsg)
find_package(VGio QUIET)
find_package(OpenMP)
# Include external dependencies
include(${PROJECT_SOURCE_DIR}/ext/external.cmake)

# Force CMake to use `-I` instead of `-isystem`
set(CMAKE_NO_SYSTEM_FROM_IMPORTED TRUE)

# Setting header files
file(GLOB HEADER_FILES "${PROJECT_SOURCE_DIR}/include/gum/*.hpp")
# Creating an INTERFACE library
add_library(gum INTERFACE)
add_library(gum_novg INTERFACE)

# If external libraries are added, the target depends on them assuring build
# order in parallel builds.
if(TARGET parallelhashmap_git)
  add_dependencies(gum parallelhashmap_git)
  add_dependencies(gum_novg parallelhashmap_git)
endif()

if(TARGET gfakluge_git)
  add_dependencies(gum gfakluge_git)
  add_dependencies(gum_novg gfakluge_git)
endif()

# Defining target sources
target_sources(gum INTERFACE "$<BUILD_INTERFACE:${HEADER_FILES}>")
target_sources(gum_novg INTERFACE "$<BUILD_INTERFACE:${HEADER_FILES}>")
# Defining include directories
target_include_directories(gum
  INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
  INTERFACE sdsl::sdsl
  INTERFACE ParallelHashmap::ParallelHashmap
  INTERFACE GFAKluge::GFAKluge
  INTERFACE libbdsg::libbdsg
  INTERFACE VGio::VGio)
target_include_directories(gum_novg
  INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
  INTERFACE sdsl::sdsl
  INTERFACE ParallelHashmap::ParallelHashmap
  INTERFACE GFAKluge::GFAKluge)
# Defining link libraries
target_link_libraries(gum
  INTERFACE sdsl::sdsl
  INTERFACE libbdsg::libbdsg
  INTERFACE VGio::VGio
  INTERFACE ParallelHashmap::ParallelHashmap
  INTERFACE GFAKluge::GFAKluge
  INTERFACE OpenMP::OpenMP_CXX)
target_link_libraries(gum_novg
  INTERFACE sdsl::sdsl
  INTERFACE ParallelHashmap::ParallelHashmap
  INTERFACE GFAKluge::GFAKluge
  INTERFACE OpenMP::OpenMP_CXX)
# Use C++17
target_compile_features(gum INTERFACE cxx_std_17)
target_compile_features(gum_novg INTERFACE cxx_std_17)
# Defining `gum::gum` and `gum::gum_novg` aliases
add_library(gum::gum ALIAS gum)
add_library(gum::gum_novg ALIAS gum_novg)
# Generating pkgconfig file
configure_file(gum.pc.in ${PROJECT_SOURCE_DIR}/gum.pc @ONLY)
# Install targets
install(FILES ${HEADER_FILES} DESTINATION include/gum)
install(FILES gum.pc DESTINATION lib/pkgconfig)

# Adding test submodule.
if(BUILD_TESTING)
  add_subdirectory(test)
endif(BUILD_TESTING)

# Adding auxiliary tools submodule.
if(BUILD_AUX_TOOLS)
  add_subdirectory(tools)
endif(BUILD_AUX_TOOLS)

# Registering tests.
include(CTest)
add_test(NAME TestUtils COMMAND gum-tests "[utils]")
add_test(NAME TestSeqGraph COMMAND gum-tests "[seqgraph]")
add_test(NAME TestIOUtils COMMAND gum-tests "[ioutils]")
add_test(NAME TestAlphabet COMMAND gum-tests "[alphabet]")
add_test(NAME TestStringSet COMMAND gum-tests "[stringset]")
