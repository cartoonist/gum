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

set(GUM_TOOLS_DEFAULT_CXXOPS -g -Wall -Wpedantic -Wno-deprecated-declarations)
if (GUM_STRICT_ON_WARNS)
  list(APPEND GUM_TOOLS_DEFAULT_CXXOPS -Werror)
endif (GUM_STRICT_ON_WARNS)

# Defining target 'gstats': Graph statistics tool
set(GSTATS_SOURCES "src/gstats.cpp")
add_executable(gstats ${GSTATS_SOURCES})
target_compile_options(gstats PRIVATE ${GUM_TOOLS_DEFAULT_CXXOPS})
target_include_directories(gstats
  PRIVATE gum::gum
  PRIVATE cxxopts::cxxopts)
target_link_libraries(gstats
  PRIVATE gum::gum
  PRIVATE cxxopts::cxxopts)
# Install targets
install(TARGETS gstats DESTINATION ${CMAKE_INSTALL_BINDIR})

# Defining target 'gbenchmark': Graph benchmarking tool
set(GBENCHMARK_SOURCES "src/gbenchmark.cpp")
add_executable(gbenchmark ${GBENCHMARK_SOURCES})
target_compile_options(gbenchmark PRIVATE ${GUM_TOOLS_DEFAULT_CXXOPS})
target_include_directories(gbenchmark
  PRIVATE gum::gum
  PRIVATE cxxopts::cxxopts)
target_link_libraries(gbenchmark
  PRIVATE gum::gum
  PRIVATE cxxopts::cxxopts)
# Install targets
install(TARGETS gbenchmark DESTINATION ${CMAKE_INSTALL_BINDIR})
