# Include external modules
include(GNUInstallDirs)
include(ExternalProject)

# Include GFAKluge in the source code
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/include/gum/internal/gfakluge.hpp)
  message(STATUS "Using bundled GFAKluge library internally")
  file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/include/gum/internal)
  set(GFAKluge_SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/gfakluge)
  ExternalProject_Add(gfakluge_git
    DOWNLOAD_COMMAND git -C ${PROJECT_SOURCE_DIR} submodule update --init --recursive -- ${GFAKluge_SOURCE_DIR}
    SOURCE_DIR ${GFAKluge_SOURCE_DIR}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make
    INSTALL_COMMAND make install PREFIX=<INSTALL_DIR>
    COMMAND ${CMAKE_COMMAND} -E copy_directory <INSTALL_DIR>/include/ ${PROJECT_SOURCE_DIR}/include/gum/internal)
endif()

if(NOT TARGET ParallelHashmap::ParallelHashmap)
  if(NOT USE_BUNDLED_PARALLEL_HASHMAP)
    message(FATAL_ERROR "Parallel Hashmap library not found. "
      "Pass in `-DUSE_BUNDLED_PARALLEL_HASHMAP=on` when running cmake to use the bundled version. "
      "It will be installed alongside the library.")
  endif()
  message(STATUS "Using bundled Parallel Hashmap library")
  set(ParallelHashmap_SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/parallel-hashmap)
  ExternalProject_Add(parallelhashmap_git
    DOWNLOAD_COMMAND git -C ${PROJECT_SOURCE_DIR} submodule update --init --recursive -- ${ParallelHashmap_SOURCE_DIR}
    SOURCE_DIR ${ParallelHashmap_SOURCE_DIR}
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>")
  ExternalProject_Get_Property(parallelhashmap_git INSTALL_DIR)
  add_library(ParallelHashmap::ParallelHashmap INTERFACE IMPORTED)
  set_target_properties(ParallelHashmap::ParallelHashmap PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "$<BUILD_INTERFACE:${INSTALL_DIR}/include>;$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")
  install(DIRECTORY ${INSTALL_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

if(ParallelHashmap_FOUND)
  set(ParallelHashmap_PKG_CFLAGS "-I${ParallelHashmap_INCLUDE_DIRS}")
endif(ParallelHashmap_FOUND)
