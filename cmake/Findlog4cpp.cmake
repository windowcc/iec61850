# find the log4cpp C++ library

find_package(PkgConfig REQUIRED)

pkg_check_modules(LOG REQUIRED orocos-log4cpp)

include_directories("${LOG_INCLUDE_DIRS}")

if("${LOG_LIBRARIES}" STREQUAL "orocos-log4cpp")
    link_libraries("${LOG_LIBRARIES}")
endif()
