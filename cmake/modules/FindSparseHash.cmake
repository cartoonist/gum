# Find 'libsparsehash' library.
#
# This set the following variables:
#   - SparseHash_FOUND
#   - SparseHash_VERSION
#   - SparseHash_INCLUDE_DIRS
#
# and the following imported targets:
#   - SparseHash::SparseHash

if(SparseHash_INCLUDE_DIRS)
  set(SparseHash_FIND_QUIETLY TRUE)
else()
  # Try pkg-config, first.
  find_package(PkgConfig QUIET)
  pkg_check_modules(SparseHash QUIET libsparsehash>=2.0.2)
  # If SparseHash_INCLUDE_DIRS is not set, this searches for the header file.
  find_path(SparseHash_INCLUDE_DIRS sparsehash/sparse_hash_map)
endif(SparseHash_INCLUDE_DIRS)

## handle the QUIETLY and REQUIRED arguments and set SparseHash_FOUND to TRUE if
## all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SparseHash DEFAULT_MSG SparseHash_INCLUDE_DIRS)

mark_as_advanced(SparseHash_FOUND SparseHash_VERSION SparseHash_INCLUDE_DIRS)

# Define `SparseHash::SparseHash` imported target
if(SparseHash_FOUND AND NOT TARGET SparseHash::SparseHash)
  add_library(SparseHash::SparseHash INTERFACE IMPORTED)
  set_target_properties(SparseHash::SparseHash PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${SparseHash_INCLUDE_DIRS}")
endif()
