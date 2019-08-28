# Prepare "Catch" library for other executables
add_subdirectory(ext/Catch2)
file(GLOB TEST_SOURCES "src/*.cpp")
configure_file(src/test_config.hpp.in ${PROJECT_BINARY_DIR}/test/src/test_config.hpp @ONLY)
include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/test/src)
add_executable(Tests ${TEST_SOURCES})
target_compile_features(Tests PRIVATE cxx_std_14)
target_compile_options(Tests PRIVATE -g -Wall -Wpedantic)
target_link_libraries(Tests Catch2::Catch2 OpenMP::OpenMP_CXX)
target_include_directories(Tests
  PRIVATE ${SPARSEHASH_INCLUDE_DIRS}
  PRIVATE VGio::VGio
  PRIVATE ${GFAKLUGE_INCLUDE_DIRS})
