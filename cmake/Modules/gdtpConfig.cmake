INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_GDTP gdtp)

FIND_PATH(
    GDTP_INCLUDE_DIRS
    NAMES gdtp/api.h
    HINTS $ENV{GDTP_DIR}/include
        ${PC_GDTP_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GDTP_LIBRARIES
    NAMES gnuradio-gdtp
    HINTS $ENV{GDTP_DIR}/lib
        ${PC_GDTP_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GDTP DEFAULT_MSG GDTP_LIBRARIES GDTP_INCLUDE_DIRS)
MARK_AS_ADVANCED(GDTP_LIBRARIES GDTP_INCLUDE_DIRS)

