# Find 'libbdsg' library.
#
# This set the following variables:
#   - libbdsg_FOUND
#   - libbdsg_VERSION
#   - libbdsg_INCLUDE_DIRS
#   - libbdsg_LIBRARIES
#
# and the following imported targets:
#   - libbdsg::libbdsg

if(libbdsg_INCLUDE_DIRS)
  set(libbdsg_FIND_QUIETLY TRUE)
else()
  # Try pkg-config, first.
  #find_package(PkgConfig QUIET)
  #pkg_check_modules(libbdsg QUIET libbdsg>=0.3)
  # If libbdsg_INCLUDE_DIRS is not set, this searches for the header/library file.
  find_path(libbdsg_INCLUDE_DIRS bdsg/hash_graph.hpp)
  find_library(libbdsg_LIBRARIES libbdsg.a)
endif(libbdsg_INCLUDE_DIRS)

## handle the QUIETLY and REQUIRED arguments and set libbdsg_FOUND to TRUE if
## all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libbdsg DEFAULT_MSG libbdsg_INCLUDE_DIRS libbdsg_LIBRARIES)

mark_as_advanced(libbdsg_FOUND libbdsg_VERSION libbdsg_INCLUDE_DIRS libbdsg_LIBRARIES)

# Define `libbdsg::libbdsg` imported target
if(libbdsg_FOUND AND NOT TARGET libbdsg::libbdsg)
  add_library(libbdsg::libbdsg INTERFACE IMPORTED)
  set_target_properties(libbdsg::libbdsg PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${libbdsg_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${libbdsg_LIBRARIES}")
endif()
