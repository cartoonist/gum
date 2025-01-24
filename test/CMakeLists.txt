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

set(GUM_TESTS_DEFAULT_CXXOPS -g -Wall -Wpedantic -Wno-deprecated-declarations)
if (GUM_STRICT_ON_WARNS)
  list(APPEND GUM_TESTS_DEFAULT_CXXOPS -Werror -Wno-c++20-extensions)
endif(GUM_STRICT_ON_WARNS)

# Defining target gum-tests
file(GLOB TEST_SOURCES "src/*.cpp")
add_executable(gum-tests ${TEST_SOURCES})

target_compile_options(gum-tests PRIVATE ${GUM_TESTS_DEFAULT_CXXOPS})
target_include_directories(gum-tests
  PRIVATE ${PROJECT_BINARY_DIR}/test/src
  PRIVATE gum::gum)
target_link_libraries(gum-tests
  PRIVATE Catch2::Catch2WithMain
  PRIVATE gum::gum)
