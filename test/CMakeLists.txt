# Prepare "Catch" library for other executables
add_subdirectory(ext/Catch2)
include_directories(${PROJECT_SOURCE_DIR}/src)
file(GLOB TEST_SOURCES "src/*.cpp")
add_executable(Tests ${TEST_SOURCES})
target_compile_features(Tests PRIVATE cxx_std_14)
target_compile_options(Tests PRIVATE -g -Wall -Wpedantic)
target_link_libraries(Tests Catch2::Catch2 OpenMP::OpenMP_CXX)
target_include_directories(Tests
  PRIVATE ${SPARSEHASH_INCLUDE_DIRS}
  PRIVATE VGio::VGio
  PRIVATE ${GFAKLUGE_INCLUDE_DIRS})
