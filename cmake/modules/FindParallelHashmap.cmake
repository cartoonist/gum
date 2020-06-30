# Find 'Parallel Hashmap' library.
#
# This set the following variables:
#   - ParallelHashmap_FOUND
#   - ParallelHashmap_INCLUDE_DIRS
#
# and the following imported targets:
#   - ParallelHashmap::ParallelHashmap

if(ParallelHashmap_INCLUDE_DIRS)
  set(ParallelHashmap_FIND_QUIETLY TRUE)
else()
  # If ParallelHashmap_INCLUDE_DIRS is not set, this searches for the header file.
  find_path(ParallelHashmap_INCLUDE_DIRS parallel_hashmap/phmap.h)
endif(ParallelHashmap_INCLUDE_DIRS)

# handle the QUIETLY and REQUIRED arguments and set ParallelHashmap_FOUND to
# TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ParallelHashmap DEFAULT_MSG ParallelHashmap_INCLUDE_DIRS)

mark_as_advanced(ParallelHashmap_INCLUDE_DIRS)

# Define `ParallelHashmap::ParallelHashmap` imported target
if(ParallelHashmap_FOUND AND NOT TARGET ParallelHashmap::ParallelHashmap)
  add_library(ParallelHashmap::ParallelHashmap INTERFACE IMPORTED)
  set_target_properties(ParallelHashmap::ParallelHashmap PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ParallelHashmap_INCLUDE_DIRS}")
endif()
