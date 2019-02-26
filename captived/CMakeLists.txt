cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)


project(http-server-libevent
  VERSION "0.0.2"
  )

option(BUILD_SHARED_LIBS "Build with shared libraries" ON)

find_package(libevent REQUIRED QUIET)
find_package(jansson REQUIRED QUIET)

## GLF debug info -- delete this
message(STATUS "#######################################")
message(STATUS "libevent_INCLUDE_DIRS = ${libevent_INCLUDE_DIRS}")
message(STATUS "jansson include dirs = ${JANSSON_INCLUDE_DIRS}")
message(STATUS "jansson libraries = ${JANSSON_LIBRARIES}")
message(STATUS "#######################################")


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(SERVER_SRCS
    captive-server-libevent.cpp
    http_server.cpp
    rest_resource_generic.cpp
    rest_resource_to_file.cpp
    rest_resource_router_mode.cpp
    )

add_executable(http-server-libevent ${SERVER_SRCS})


# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

target_include_directories(http-server-libevent
    PUBLIC include
    PUBLIC ${Boost_INCLUDE_DIRS}
    PUBLIC ${Boost_SYSTEM_INCLUDE_DIRS}
    PUBLIC ${libevent_INCLUDE_DIRS}
    PUBLIC ${JANSSON_INCLUDE_DIRS}
    )

target_link_libraries(http-server-libevent
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC ${libevent_LIBRARIES}
    PUBLIC ${JANSSON_LIBRARIES}
    )

set_target_properties(http-server-libevent PROPERTIES
    OUTPUT_NAME http-server-libevent
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    )


