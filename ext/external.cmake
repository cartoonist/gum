# Include external modules
include(GNUInstallDirs)
include(ExternalProject)

# Include GFAKluge in the source code
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/include/gum/internal/gfakluge.hpp)
  message(STATUS "Embedding GFAKluge library")
  file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/include/gum/internal)
  set(GFAKluge_SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/gfakluge)
  ExternalProject_Add(gfakluge_git
    DOWNLOAD_COMMAND git -C ${PROJECT_SOURCE_DIR} submodule update --init --recursive -- ${GFAKluge_SOURCE_DIR}
    SOURCE_DIR ${GFAKluge_SOURCE_DIR}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo "Skipping configure step..."
    BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping build step..."
    INSTALL_COMMAND make install-headers PREFIX=<INSTALL_DIR>
    COMMAND ${CMAKE_COMMAND} -E copy_directory <INSTALL_DIR>/include/ ${PROJECT_SOURCE_DIR}/include/gum/internal)
endif()

# Include `xxsds/sdsl-lite` in the source code
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/include/gum/internal/sdsl/config.hpp)
  message(STATUS "Embedding sdsl-lite library")
  file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/include/gum/internal/sdsl)
  set(SDSL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/sdsl-lite)
  ExternalProject_Add(sdsl_git
    DOWNLOAD_COMMAND git -C ${PROJECT_SOURCE_DIR} submodule update --init --recursive -- ${SDSL_SOURCE_DIR}
    SOURCE_DIR ${SDSL_SOURCE_DIR}
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo "Skipping configure step..."
    BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping build step..."
    INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDSL_SOURCE_DIR}/include/sdsl/ ${PROJECT_SOURCE_DIR}/include/gum/internal/sdsl)
endif()

if(NOT EXISTS ${PROJECT_SOURCE_DIR}/include/gum/internal/parallel_hashmap/phmap.h)
  message(STATUS "Embedding Parallel Hashmap library")
  file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/include/gum/internal/parallel_hashmap)
  set(ParallelHashmap_SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/parallel-hashmap)
  ExternalProject_Add(parallelhashmap_git
    DOWNLOAD_COMMAND git -C ${PROJECT_SOURCE_DIR} submodule update --init --recursive -- ${ParallelHashmap_SOURCE_DIR}
    SOURCE_DIR ${ParallelHashmap_SOURCE_DIR}
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo "Skipping configure step..."
    BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping build step..."
    INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ParallelHashmap_SOURCE_DIR}/parallel_hashmap/ ${PROJECT_SOURCE_DIR}/include/gum/internal/parallel_hashmap)
endif()
