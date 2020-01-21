# Find 'GFAKluge' library.
#
# This set the following variables:
#   - GFAKLUGE_FOUND
#   - GFAKLUGE_INCLUDE_DIRS
#
# and the following imported targets:
#   - GFAKluge::GFAKluge

if(GFAKLUGE_INCLUDE_DIRS)
  set(GFAKLUGE_FIND_QUIETLY TRUE)
else()
  # If GFAKLUGE_INCLUDE_DIRS is not set, this searches for the header file.
  find_path(GFAKLUGE_INCLUDE_DIRS gfakluge.hpp)
endif(GFAKLUGE_INCLUDE_DIRS)

# handle the QUIETLY and REQUIRED arguments and set GFAKLUGE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GFAKluge DEFAULT_MSG GFAKLUGE_INCLUDE_DIRS)

mark_as_advanced(GFAKLUGE_INCLUDE_DIRS)

# Define `GFAKluge::GFAKluge` imported target
if(GFAKLUGE_FOUND AND NOT TARGET GFAKluge::GFAKluge)
  add_library(GFAKluge::GFAKluge INTERFACE IMPORTED)
  set_target_properties(GFAKluge::GFAKluge PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${GFAKLUGE_INCLUDE_DIRS}")
endif()
