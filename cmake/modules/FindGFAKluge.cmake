# Find GFAKluge library and set GFAKLUGE_FOUND and define
# GFAKLUGE_INCLUDE_DIRS as the GFAKluge include directories.

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
