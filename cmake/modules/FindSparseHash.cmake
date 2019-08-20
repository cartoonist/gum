# Find libsparsehash library and set SPARSEHASH_FOUND and define
# SPARSEHASH_INCLUDE_DIRS as the sparsehash include directories.

if(SPARSEHASH_INCLUDE_DIRS)
  set(SPARSEHASH_FIND_QUIETLY TRUE)
else()
  # Try pkg-config, first.
  find_package(PkgConfig QUIET)
  pkg_check_modules(SPARSEHASH REQUIRED libsparsehash>=2.0.2)
  # If SPARSEHASH_INCLUDE_DIRS is not set, this searches for the header file.
  find_path(SPARSEHASH_INCLUDE_DIRS sparsehash/sparse_hash_map)
endif(SPARSEHASH_INCLUDE_DIRS)

## handle the QUIETLY and REQUIRED arguments and set SPARSEHASH_FOUND to TRUE if
## all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libsparsehash DEFAULT_MSG SPARSEHASH_INCLUDE_DIRS)

mark_as_advanced(SPARSEHASH_INCLUDE_DIRS)