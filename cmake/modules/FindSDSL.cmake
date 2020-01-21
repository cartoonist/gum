# Find 'sdsl-lite' library.
#
# This set the following variables:
#   - SDSL_FOUND
#   - SDSL_VERSION
#   - SDSL_INCLUDE_DIRS
#   - SDSL_LIBRARIES
#
# and the following imported targets:
#   - sdsl::sdsl

if(SDSL_INCLUDE_DIRS)
  set(SDSL_FIND_QUIETLY TRUE)
else()
  # Try pkg-config, first.
  find_package(PkgConfig QUIET)
  pkg_check_modules(SDSL QUIET sdsl-lite>=2.1.0)
  # If SDSL_INCLUDE_DIRS is not set, this searches for the header/library file.
  find_path(SDSL_INCLUDE_DIRS sdsl/config.hpp)
  find_library(SDSL_LIBRARIES sdsl)
endif(SDSL_INCLUDE_DIRS)

## handle the QUIETLY and REQUIRED arguments and set SDSL_FOUND to TRUE if
## all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(sdsl-lite DEFAULT_MSG SDSL_INCLUDE_DIRS SDSL_LIBRARIES)

mark_as_advanced(SDSL_FOUND SDSL_VERSION SDSL_INCLUDE_DIRS SDSL_LIBRARIES)

# Define `sdsl::sdsl` imported target
if(SDSL_FOUND AND NOT TARGET sdsl::sdsl)
  add_library(sdsl::sdsl INTERFACE IMPORTED)
  set_target_properties(sdsl::sdsl PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${SDSL_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${SDSL_LIBRARIES}")
endif()
