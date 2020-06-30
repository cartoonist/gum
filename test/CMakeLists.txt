# Enabling `assert` by setting the build type to 'Debug' for test module
set(CMAKE_BUILD_TYPE "Debug")

# Prepare "Catch" library for other executables
if(NOT TARGET Catch2::Catch2)
  set(CATCH_SOURCE_DIR ext/catch2)
  execute_process(COMMAND git submodule update --init -- ${CATCH_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_subdirectory(${CATCH_SOURCE_DIR})
endif()

# Defining the configure header file
configure_file(src/test_config.hpp.in ${PROJECT_BINARY_DIR}/test/src/test_config.hpp @ONLY)

# Defining target gum-tests
file(GLOB TEST_SOURCES "src/*.cpp")
add_executable(gum-tests ${TEST_SOURCES})

# If external libraries are added, the target depends on them assuring build
# order in parallel builds.
if(TARGET parallelhashmap_git)
  add_dependencies(gum-tests parallelhashmap_git)
endif()
if(TARGET gfakluge_git)
  add_dependencies(gum-tests gfakluge_git)
endif()

target_compile_options(gum-tests PRIVATE -g -Wall -Wpedantic -Werror)
target_include_directories(gum-tests
  PRIVATE ${PROJECT_BINARY_DIR}/test/src
  PRIVATE gum::gum)
target_link_libraries(gum-tests
  PRIVATE Catch2::Catch2
  PRIVATE gum::gum)
