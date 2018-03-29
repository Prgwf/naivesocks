cmake_minimum_required(VERSION 3.9)
project(naivesocks)

if (UNIX)
    add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif (UNIX)

# dependencies
# ===================================
#find_path(LIBEVENT_INCLUDE_DIR event2/event.h REQUIRED)
#find_package(LIBEVENT REQUIRED)
set(LibEvent_EXTRA_PREFIXES /usr/local /opt/local "$ENV{HOME}")
foreach(prefix ${LibEvent_EXTRA_PREFIXES})
    list(APPEND LibEvent_INCLUDE_PATHS "${prefix}/include")
    list(APPEND LibEvent_LIB_PATHS "${prefix}/lib")
endforeach()

find_path(LIBEVENT_INCLUDE_DIR event.h PATHS ${LibEvent_INCLUDE_PATHS})
find_library(LIBEVENT_LIB NAMES event PATHS ${LibEvent_LIB_PATHS})

if (LIBEVENT_LIB AND LIBEVENT_INCLUDE_DIR)
    set(LibEvent_FOUND TRUE)
    set(LIBEVENT_LIB ${LIBEVENT_LIB})
else ()
    set(LibEvent_FOUND FALSE)
endif ()

if (LibEvent_FOUND)
    if (NOT LibEvent_FIND_QUIETLY)
        message(STATUS "Found libevent: ${LIBEVENT_LIB}")
    endif ()
else ()
    if (LibEvent_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find libevent.")
    endif ()
    message(STATUS "libevent NOT found.")
endif ()

mark_as_advanced(
        LIBEVENT_LIB
        LIBEVENT_INCLUDE_DIR
)
# ===================================

set(CMAKE_CXX_STANDARD 11)
set(LEVENT_LIBRARIES "-levent")

#target_link_libraries(naivesocks ${LIBEVENT_LIBRARIES})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -Werror -march=native")

# output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)


# includes
set(NAIVESOCKS_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
include_directories(${NAIVESOCKS_SRC_INCLUDE_DIR})
include_directories(BEFORE src)

# sub directories
add_subdirectory(src)