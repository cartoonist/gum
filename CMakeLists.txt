cmake_minimum_required(VERSION 3.19)

# Adding CMake module path.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

# Options
option(BUILD_TESTING "Build test programs" OFF)
option(BUILD_GUM_AUX_TOOLS "Build GUM auxiliary tools" OFF)
option(WITH_VG "With vg graph loading support" ON)
option(WITH_HG "With HashGraph loading support" ON)
option(WITH_VGIO "With io support for vg files" OFF)
option(WITH_BDSG "With io support for HashGraph files" OFF)
option(USE_VCPKG "Use vcpkg for installing dependencies" OFF)
option(PROTOBUF_AS_DEFAULT_VG "Consider Protobuf vg as the default format for 'vg' files" ON)

# Processing options
if(WITH_VGIO)
  set(WITH_VG on)
endif(WITH_VGIO)

if(WITH_BDSG)
  set(WITH_HG on)
endif(WITH_BDSG)

if(BUILD_TESTING AND NOT (WITH_VGIO AND WITH_BDSG))
  message(FATAL_ERROR "Building tests requires `libvgio` and `libbdsg`. "
    "Either enable VG and BDSG support by defining `WITH_VGIO=on` and "
    "`WITH_BDSG=on` or disable building tests (`BUILD_TESTING=off`).")
endif()

if(BUILD_GUM_AUX_TOOLS AND NOT (WITH_VGIO AND WITH_BDSG))
  message(FATAL_ERROR "Building auxiliary tools requires `libvgio` and `libbdsg`. "
    "Either enable VG and BDSG support by defining `WITH_VGIO=on` and "
    "`WITH_BDSG=on` or disable building auxiliary tools (`BUILD_GUM_AUX_TOOLS=off`).")
endif()

if(WITH_VGIO)
  list(APPEND VCPKG_MANIFEST_FEATURES "vgio")
endif(WITH_VGIO)

if(WITH_BDSG)
  list(APPEND VCPKG_MANIFEST_FEATURES "bdsg")
endif(WITH_BDSG)

# Load AutoVcpkg module
if(USE_VCPKG)
  include(AutoVcpkg)
  vcpkg_lazy_configure()
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE FILEPATH "Vcpkg toolchain file")
endif(USE_VCPKG)

# Defining the project
project(gum VERSION 1.0.0 LANGUAGES CXX)

# Include external modules
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Finding dependencies
find_package(ZLIB REQUIRED)
find_package(OpenMP)
# Optional dependencies
if(WITH_VGIO)
  find_package(VGio CONFIG REQUIRED)
endif(WITH_VGIO)
if(WITH_BDSG)
  if (USE_VCPKG)
    # Use CMake config files
    find_package(libbdsg CONFIG REQUIRED)
  else(USE_VCPKG)
    # Use find module script
    find_package(libbdsg REQUIRED)
  endif(USE_VCPKG)
endif(WITH_BDSG)
# Handle embedded dependencies
include(${PROJECT_SOURCE_DIR}/ext/external.cmake)

# Creating the INTERFACE library
add_library(gum INTERFACE)

# The target should depend on embedded dependencies targets assuring proper
# build order in parallel builds.
if(TARGET parallelhashmap_git)
  add_dependencies(gum parallelhashmap_git)
endif()
if(TARGET gfakluge_git)
  add_dependencies(gum gfakluge_git)
endif()
if(TARGET sdsl_git)
  add_dependencies(gum sdsl_git)
endif()

# Defining include directories
target_include_directories(gum
  INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>;$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
  INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/gum/internal>;$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}/gum/internal>)

if(WITH_VGIO)
  target_include_directories(gum
    INTERFACE $<TARGET_PROPERTY:VGio::VGio,INTERFACE_INCLUDE_DIRECTORIES>)
endif(WITH_VGIO)
if(WITH_BDSG)
  target_include_directories(gum
    INTERFACE $<TARGET_PROPERTY:libbdsg::libbdsg,INTERFACE_INCLUDE_DIRECTORIES>)
endif(WITH_BDSG)
# Defining link libraries
target_link_libraries(gum
  INTERFACE $<BUILD_INTERFACE:OpenMP::OpenMP_CXX>;$<INSTALL_INTERFACE:OpenMP::OpenMP_CXX>)
if(WITH_VGIO)
  target_link_libraries(gum
    INTERFACE $<BUILD_INTERFACE:VGio::VGio>;$<INSTALL_INTERFACE:VGio::VGio>)
endif(WITH_VGIO)
if(WITH_BDSG)
  target_link_libraries(gum
    INTERFACE $<BUILD_INTERFACE:libbdsg::libbdsg>;$<INSTALL_INTERFACE:libbdsg::libbdsg>)
endif(WITH_BDSG)
# Use C++17
target_compile_features(gum INTERFACE cxx_std_17)
# Generating the configure header file
set(GUM_IO_PROTOBUF_VG "${PROTOBUF_AS_DEFAULT_VG}")
set(GUM_HAS_VG "${WITH_VG}")
set(GUM_HAS_HG "${WITH_HG}")
set(GUM_HAS_VGIO "${WITH_VGIO}")
set(GUM_HAS_BDSG "${WITH_BDSG}")
configure_file(include/gum/config.hpp.in ${PROJECT_SOURCE_DIR}/include/gum/config.hpp @ONLY)
# Generating pkgconfig file
configure_file(gum.pc.in ${PROJECT_SOURCE_DIR}/gum.pc @ONLY)
# Setting header files
file(GLOB HEADER_FILES RELATIVE "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/include/gum/*.hpp")
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${PROJECT_SOURCE_DIR}/include/gum/internal")
if(NOT WITH_VG)
  list(REMOVE_ITEM HEADER_FILES "gum/vg_utils.hpp")
endif(NOT WITH_VG)
if(NOT WITH_HG)
  list(REMOVE_ITEM HEADER_FILES "gum/hg_utils.hpp")
endif(NOT WITH_HG)
if(NOT WITH_VGIO)
  list(REMOVE_ITEM HEADER_FILES "gum/vgio_utils.hpp")
endif(NOT WITH_VGIO)
if(NOT WITH_BDSG)
  list(REMOVE_ITEM HEADER_FILES "gum/bdsg_utils.hpp")
endif(NOT WITH_BDSG)
list(TRANSFORM HEADER_FILES PREPEND "${PROJECT_SOURCE_DIR}/include/" OUTPUT_VARIABLE BUILD_HEADER_FILES)
list(TRANSFORM HEADER_FILES PREPEND "${CMAKE_INSTALL_FULL_INCLUDEDIR}/" OUTPUT_VARIABLE INSTALL_HEADER_FILES)
# Defining target sources
target_sources(gum
  INTERFACE "$<BUILD_INTERFACE:${BUILD_HEADER_FILES}>;$<INSTALL_INTERFACE:${INSTALL_HEADER_FILES}>")
# Defining `gum::gum` alias
add_library(gum::gum ALIAS gum)
# Install targets
install(FILES ${BUILD_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/gum)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/gum/internal" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/gum)
install(FILES gum.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
# Exporting targets
install(TARGETS gum EXPORT gum-targets)
install(EXPORT gum-targets NAMESPACE gum:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gum)
# Creating the package configuration file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gum-config.cmake.in
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

# Packaging configuration
set(CPACK_PACKAGE_NAME "gum")
set(CPACK_PACKAGE_VENDOR "cartoonist")
set(CPACK_GENERATOR "STGZ;TGZ;RPM")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A header-only library for representation of sequence graphs")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_SYSTEM_NAME "noarch")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
include(CPack)
