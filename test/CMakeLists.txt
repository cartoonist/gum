# Enabling `assert` by setting the build type to 'Debug' for test module
set(CMAKE_BUILD_TYPE "Debug")

# Prepare "Catch" library for other executables
add_subdirectory(ext/catch2)
file(GLOB TEST_SOURCES "src/*.cpp")
configure_file(src/test_config.hpp.in ${PROJECT_BINARY_DIR}/test/src/test_config.hpp @ONLY)
include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/test/src)
add_executable(Tests ${TEST_SOURCES})
target_compile_features(Tests PRIVATE cxx_std_17)
target_compile_options(Tests PRIVATE -g -Wall -Wpedantic -Werror)
target_link_libraries(Tests Catch2::Catch2 ${SDSL_LIBRARIES} VGio::VGio OpenMP::OpenMP_CXX)
target_include_directories(Tests
  PRIVATE ${SPARSEHASH_INCLUDE_DIRS}
  PRIVATE ${SDSL_INCLUDE_DIRS}
  PRIVATE VGio::VGio
  PRIVATE ${GFAKLUGE_INCLUDE_DIRS})
