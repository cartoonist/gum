# Prepare "Catch" library for other executables
add_subdirectory(ext/Catch2)
include_directories(${PROJECT_SOURCE_DIR}/src)
file(GLOB TEST_SOURCES "src/*.cpp")
add_executable(Tests ${TEST_SOURCES})
target_compile_features(Tests PRIVATE cxx_std_14)
target_compile_options(Tests PRIVATE -Wall -Wpedantic -Werror)
target_link_libraries(Tests Catch2::Catch2)
