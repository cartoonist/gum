# Include external modules
include(GNUInstallDirs)
include(ExternalProject)

if(ParallelHashmap_FOUND)
  set(ParallelHashmap_PKG_CFLAGS "-I${ParallelHashmap_INCLUDE_DIRS}")
endif(ParallelHashmap_FOUND)

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

if(GFAKluge_FOUND)
  set(GFAKluge_PKG_CFLAGS "-I${GFAKluge_INCLUDE_DIRS}")
endif(GFAKluge_FOUND)

if(NOT TARGET GFAKluge::GFAKluge)
  if(NOT USE_BUNDLED_GFAKLUGE)
    message(FATAL_ERROR "GFAKluge library not found. "
      "Pass in `-DUSE_BUNDLED_GFAKLUGE=on` when running cmake to use the bundled version. "
      "It will be installed alongside the library.")
  endif()
  message(STATUS "Using bundled GFAKluge library")
  set(GFAKluge_SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/gfakluge)
  ExternalProject_Add(gfakluge_git
    DOWNLOAD_COMMAND git -C ${PROJECT_SOURCE_DIR} submodule update --init --recursive -- ${GFAKluge_SOURCE_DIR}
    SOURCE_DIR ${GFAKluge_SOURCE_DIR}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make
    INSTALL_COMMAND make install PREFIX=<INSTALL_DIR>)
  ExternalProject_Get_Property(gfakluge_git INSTALL_DIR)
  add_library(GFAKluge::GFAKluge INTERFACE IMPORTED)
  set_target_properties(GFAKluge::GFAKluge PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "$<BUILD_INTERFACE:${INSTALL_DIR}/include>;$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")
  install(DIRECTORY ${INSTALL_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()
