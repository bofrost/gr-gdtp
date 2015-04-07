# - Try to find libgdtp - https://github.com/andrepuschmann/libgdtp
# Once done this will define
#  GDTP_FOUND - System has gdtp
#  GDTP_INCLUDE_DIRS - The libgdtp include directories
#  GDTP_LIBRARIES - The libraries needed to use libgdtp

find_path(GDTP_INCLUDE_DIR
            NAMES libgdtp.h
            HINTS $ENV{GDTP_DIR}/include/libgdtp
            PATHS /usr/local/include/libgdtp
                  /usr/include/libgdtp )

find_library(GDTP_LIBRARY
            NAMES gdtp
            HINTS $ENV{GDTP_DIR}/lib
            PATHS /usr/local/lib
                  /usr/lib)

set(GDTP_LIBRARIES ${GDTP_LIBRARY} )
set(GDTP_INCLUDE_DIRS ${GDTP_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GDTP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(gdtp  DEFAULT_MSG GDTP_LIBRARY GDTP_INCLUDE_DIR)
mark_as_advanced(GDTP_INCLUDE_DIR GDTP_LIBRARY )
