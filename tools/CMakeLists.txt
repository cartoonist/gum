# Include external modules
include(GNUInstallDirs)

# Setting the build type if it is not already set
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Prepare "cxxopts" dependency
if(NOT TARGET cxxopts::cxxopts)
  set(CXXOPTS_SOURCE_DIR ext/cxxopts)
  execute_process(COMMAND git submodule update --init -- ${CXXOPTS_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_subdirectory(${CXXOPTS_SOURCE_DIR} EXCLUDE_FROM_ALL)
endif()

# Defining target 'gstats': Graph statistics tool
set(GSTATS_SOURCES "src/gstats.cpp")
add_executable(gstats ${GSTATS_SOURCES})
target_compile_options(gstats PRIVATE -g -Wall -Wpedantic -Werror)
target_include_directories(gstats
  PRIVATE gum::gum
  PRIVATE cxxopts::cxxopts)
target_link_libraries(gstats
  PRIVATE gum::gum
  PRIVATE cxxopts::cxxopts)
# Install targets
install(TARGETS gstats DESTINATION ${CMAKE_INSTALL_BINDIR})
