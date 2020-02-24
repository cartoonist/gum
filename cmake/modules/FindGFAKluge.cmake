# Find 'GFAKluge' library.
#
# This set the following variables:
#   - GFAKluge_FOUND
#   - GFAKluge_INCLUDE_DIRS
#
# and the following imported targets:
#   - GFAKluge::GFAKluge

if(GFAKluge_INCLUDE_DIRS)
  set(GFAKluge_FIND_QUIETLY TRUE)
else()
  # If GFAKluge_INCLUDE_DIRS is not set, this searches for the header file.
  find_path(GFAKluge_INCLUDE_DIRS gfakluge.hpp)
endif(GFAKluge_INCLUDE_DIRS)

# handle the QUIETLY and REQUIRED arguments and set GFAKluge_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GFAKluge DEFAULT_MSG GFAKluge_INCLUDE_DIRS)

mark_as_advanced(GFAKluge_INCLUDE_DIRS)

# Define `GFAKluge::GFAKluge` imported target
if(GFAKluge_FOUND AND NOT TARGET GFAKluge::GFAKluge)
  add_library(GFAKluge::GFAKluge INTERFACE IMPORTED)
  set_target_properties(GFAKluge::GFAKluge PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${GFAKluge_INCLUDE_DIRS}")
endif()
