# - Find LDM
# Find the LDM library
#
#  This module defines the following variables:
#     LDM_FOUND   - True if LDM_INCLUDE is found
#     LDM_INCLUDE - where to find header files
#     LDM_LIB     - the library files


if (NOT LDM_FOUND)
    find_path (LDM_INCLUDE
               NAMES "ldm/Device"
               PATH_SUFFIXES include
               DOC "LDM include directory")

    find_library (LDM_LIB
                  NAMES ldm
                  DOC "LDM library file")

    # handle the QUIETLY and REQUIRED arguments and set LDM_FOUND to TRUE if all listed variables are TRUE
    include (FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS (LDM DEFAULT_MSG LDM_INCLUDE LDM_LIB)

    mark_as_advanced (LDM_INCLUDE LDM_LIB)
endif ()
