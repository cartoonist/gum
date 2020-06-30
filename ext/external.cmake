include(ExternalProject)

if(ParallelHashmap_FOUND)
  set(ParallelHashmap_PKG_CFLAGS "-I${ParallelHashmap_INCLUDE_DIRS}")
endif(ParallelHashmap_FOUND)

if(NOT TARGET ParallelHashmap::ParallelHashmap)
  set(ParallelHashmap_SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/parallel-hashmap)
  ExternalProject_Add(parallelhashmap_git
    DOWNLOAD_COMMAND git -C ${PROJECT_SOURCE_DIR} submodule update --init --recursive -- ${ParallelHashmap_SOURCE_DIR}
    SOURCE_DIR ${ParallelHashmap_SOURCE_DIR}
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>")
  ExternalProject_Get_Property(parallelhashmap_git INSTALL_DIR)
  add_library(ParallelHashmap::ParallelHashmap INTERFACE IMPORTED)
  set_target_properties(ParallelHashmap::ParallelHashmap PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "$<BUILD_INTERFACE:${INSTALL_DIR}/include>;$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>")
  install(DIRECTORY ${INSTALL_DIR}/include/ DESTINATION include)
endif()

if(GFAKluge_FOUND)
  set(GFAKluge_PKG_CFLAGS "-I${GFAKluge_INCLUDE_DIRS}")
endif(GFAKluge_FOUND)

if(NOT TARGET GFAKluge::GFAKluge)
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
    INTERFACE_INCLUDE_DIRECTORIES "$<BUILD_INTERFACE:${INSTALL_DIR}/include>;$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>")
  install(DIRECTORY ${INSTALL_DIR}/include/ DESTINATION include)
endif()
