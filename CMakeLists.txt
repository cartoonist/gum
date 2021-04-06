cmake_minimum_required(VERSION 3.10)
project(gum VERSION 0.1.0 LANGUAGES CXX)

# Options
option(BUILD_TESTING "Build test programs" OFF)
option(BUILD_GUM_AUX_TOOLS "Build GUM auxiliary tools" OFF)
option(BUILD_WITH_VG "Build with VG support" ON)
option(BUILD_WITH_BDSG "Build with BDSG support" OFF)
option(USE_BUNDLED_PARALLEL_HASHMAP "Use bundled Parallel Hashmap" OFF)
option(USE_BUNDLED_GFAKLUGE "Use bundled GFAKluge" OFF)
option(PROTOBUF_AS_DEFAULT_VG "Consider Protobuf vg as the default format for 'vg' files" ON)

if(BUILD_TESTING AND NOT (BUILD_WITH_VG AND BUILD_WITH_BDSG))
  message(FATAL_ERROR "Building tests requires `libvgio` and `libbdsg`. "
    "Either enable VG and BDSG support by defining `BUILD_WITH_VG=on` and "
    "`BUILD_WITH_BDSG=on` or disable building tests (`BUILD_TESTING=off`).")
endif()

if(BUILD_GUM_AUX_TOOLS AND NOT (BUILD_WITH_VG AND BUILD_WITH_BDSG))
  message(FATAL_ERROR "Building auxiliary tools requires `libvgio` and `libbdsg`. "
    "Either enable VG and BDSG support by defining `BUILD_WITH_VG=on` and "
    "`BUILD_WITH_BDSG=on` or disable building auxiliary tools (`BUILD_GUM_AUX_TOOLS=off`).")
endif()

# Include external modules
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
# Adding CMake module path.
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
# Finding dependencies.
find_package(ZLIB REQUIRED)
find_package(sdsl REQUIRED)
find_package(OpenMP)
# Bundled dependencies
find_package(ParallelHashmap)
find_package(GFAKluge)
# Optional dependencies
if(BUILD_WITH_VG)
  find_package(VGio REQUIRED)
endif(BUILD_WITH_VG)
if(BUILD_WITH_BDSG)
  find_package(libbdsg REQUIRED)
endif(BUILD_WITH_BDSG)
# Handle bundled dependencies
include(${PROJECT_SOURCE_DIR}/ext/external.cmake)

# Creating an INTERFACE library
add_library(gum INTERFACE)

# If bundled libraries are used, the target should depend on them assuring build order
# in parallel builds.
if(USE_BUNDLED_PARALLEL_HASHMAP AND TARGET parallelhashmap_git)
  add_dependencies(gum parallelhashmap_git)
endif()

if(USE_BUNDLED_GFAKLUGE AND TARGET gfakluge_git)
  add_dependencies(gum gfakluge_git)
endif()

# Defining include directories
target_include_directories(gum
  INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>;$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
  INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdsl::sdsl>;$<INSTALL_INTERFACE:sdsl::sdsl>
  INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/ParallelHashmap::ParallelHashmap>;$<INSTALL_INTERFACE:ParallelHashmap::ParallelHashmap>
  INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/GFAKluge::GFAKluge>;$<INSTALL_INTERFACE:GFAKluge::GFAKluge>)
if(BUILD_WITH_VG)
  target_include_directories(gum
    INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/VGio::VGio>;$<INSTALL_INTERFACE:VGio::VGio>)
endif(BUILD_WITH_VG)
if(BUILD_WITH_BDSG)
  target_include_directories(gum
    INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/libbdsg::libbdsg>;$<INSTALL_INTERFACE:libbdsg::libbdsg>)
endif(BUILD_WITH_BDSG)
# Defining link libraries
target_link_libraries(gum
  INTERFACE $<BUILD_INTERFACE:sdsl::sdsl>;$<INSTALL_INTERFACE:sdsl::sdsl>
  INTERFACE $<BUILD_INTERFACE:ParallelHashmap::ParallelHashmap>;$<INSTALL_INTERFACE:ParallelHashmap::ParallelHashmap>
  INTERFACE $<BUILD_INTERFACE:GFAKluge::GFAKluge>;$<INSTALL_INTERFACE:GFAKluge::GFAKluge>
  INTERFACE $<BUILD_INTERFACE:OpenMP::OpenMP_CXX>;$<INSTALL_INTERFACE:OpenMP::OpenMP_CXX>)
if(BUILD_WITH_VG)
  target_link_libraries(gum
    INTERFACE $<BUILD_INTERFACE:VGio::VGio>;$<INSTALL_INTERFACE:VGio::VGio>)
endif(BUILD_WITH_VG)
if(BUILD_WITH_BDSG)
  target_link_libraries(gum
    INTERFACE $<BUILD_INTERFACE:libbdsg::libbdsg>;$<INSTALL_INTERFACE:libbdsg::libbdsg>)
endif(BUILD_WITH_BDSG)
# Use C++17
target_compile_features(gum INTERFACE cxx_std_17)
# Generating the configure header file
set(GUM_IO_PROTOBUF_VG "${PROTOBUF_AS_DEFAULT_VG}")
set(GUM_WITH_VG "${BUILD_WITH_VG}")
set(GUM_WITH_BDSG "${BUILD_WITH_BDSG}")
configure_file(include/gum/config.hpp.in ${PROJECT_SOURCE_DIR}/include/gum/config.hpp @ONLY)
# Generating pkgconfig file
configure_file(gum.pc.in ${PROJECT_SOURCE_DIR}/gum.pc @ONLY)
# Setting header files
file(GLOB HEADER_FILES RELATIVE "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/include/gum/*.hpp")
if(NOT BUILD_WITH_VG)
  list(REMOVE_ITEM HEADER_FILES "gum/vg_utils.hpp")
endif(NOT BUILD_WITH_VG)
if(NOT BUILD_WITH_BDSG)
  list(REMOVE_ITEM HEADER_FILES "gum/hg_utils.hpp")
endif(NOT BUILD_WITH_BDSG)
list(TRANSFORM HEADER_FILES PREPEND "${PROJECT_SOURCE_DIR}/include/" OUTPUT_VARIABLE BUILD_HEADER_FILES)
list(TRANSFORM HEADER_FILES PREPEND "${CMAKE_INSTALL_FULL_INCLUDEDIR}/" OUTPUT_VARIABLE INSTALL_HEADER_FILES)
# Defining target sources
target_sources(gum
  INTERFACE "$<BUILD_INTERFACE:${BUILD_HEADER_FILES}>;$<INSTALL_INTERFACE:${INSTALL_HEADER_FILES}>")
# Defining `gum::gum` alias
add_library(gum::gum ALIAS gum)
# Install targets
install(FILES ${BUILD_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/gum)
install(FILES gum.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
# Exporting targets
install(TARGETS gum EXPORT gum-targets)
install(EXPORT gum-targets NAMESPACE gum:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gum)
# Creating the package configuration file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/gum-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gum)
# Generating the version file for the configuration file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/gum-config-version.cmake"
  VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
  COMPATIBILITY AnyNewerVersion)
# Install generated configuration files
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/gum-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/gum-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gum)
# Install custom find modules
file(GLOB GUM_FIND_MODULE_FILES "${PROJECT_SOURCE_DIR}/cmake/modules/Find*.cmake")
install(FILES ${GUM_FIND_MODULE_FILES} DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gum)

# Adding test submodule.
if(BUILD_TESTING)
  add_subdirectory(test)
endif(BUILD_TESTING)

# Adding auxiliary tools submodule.
if(BUILD_GUM_AUX_TOOLS)
  add_subdirectory(tools)
endif(BUILD_GUM_AUX_TOOLS)

# Registering tests.
include(CTest)
add_test(NAME TestUtils COMMAND gum-tests "[utils]")
add_test(NAME TestSeqGraph COMMAND gum-tests "[seqgraph]")
add_test(NAME TestIOUtils COMMAND gum-tests "[ioutils]")
add_test(NAME TestAlphabet COMMAND gum-tests "[alphabet]")
add_test(NAME TestStringSet COMMAND gum-tests "[stringset]")
