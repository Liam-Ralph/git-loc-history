find_path(GIT2_INCLUDE_PATH NAMES git2.h HINTS "${CMAKE_SOURCE_DIR}/libgit2-1.9.7/build/include")
find_library(GIT2_LIBRARY NAMES git2 HINTS "${CMAKE_SOURCE_DIR}/libgit2-1.9.7/build")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(git2 REQUIRED_VARS GIT2_LIBRARY GIT2_INCLUDE_PATH)

if (GIT2_FOUND)
    set(GIT2_INCLUDE_DIR ${GIT2_INCLUDE_PATH})
    set(GIT2_INCLUDE_DIRS ${GIT2_INCLUDE_PATH})
    set(GIT2_LIBRARIES ${GIT2_LIBRARY})
endif()

mark_as_advanced(
    GIT2_INCLUDE_PATH
    GIT2_LIBRARY
)
