# Find sdsl-lite library and set SDSL_FOUND and define SDSL_INCLUDE_DIRS
# as the sdsl-lite include directories and SDSL_LIBRARIES as the
# sdsl-lite libraries.

if(SDSL_INCLUDE_DIRS)
  set(SDSL_FIND_QUIETLY TRUE)
else()
  # Try pkg-config, first.
  find_package(PkgConfig QUIET)
  pkg_check_modules(SDSL REQUIRED sdsl-lite>=2.1.0)
  # If SDSL_INCLUDE_DIRS is not set, this searches for the header/library file.
  find_path(SDSL_INCLUDE_DIRS sdsl/config.hpp)
  find_library(SDSL_LIBRARIES sdsl)
endif(SDSL_INCLUDE_DIRS)

## handle the QUIETLY and REQUIRED arguments and set SDSL_FOUND to TRUE if
## all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(sdsl-lite DEFAULT_MSG SDSL_INCLUDE_DIRS SDSL_LIBRARIES)

mark_as_advanced(SDSL_INCLUDE_DIRS SDSL_LIBRARIES)
