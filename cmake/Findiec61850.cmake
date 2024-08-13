# find the iec 61850 C++ library

find_package(PkgConfig REQUIRED)

pkg_check_modules(IEC61850 REQUIRED libiec61850)

include_directories("${IEC61850_INCLUDE_DIRS}")

if("${IEC61850_LIBRARIES}" STREQUAL "libiec61850")
    link_libraries("${libiec61850}")
endif()
