cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

project(captived
  VERSION "0.0.2"
  )

include(GNUInstallDirs)

option(BUILD_SHARED_LIBS "Build with shared libraries" ON)

find_package(libevent REQUIRED QUIET)
find_package(jansson REQUIRED QUIET)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(SERVER_SRCS
    main.cpp
    http_server.cpp
    rest_resource_generic.cpp
    rest_resource_to_file.cpp
    rest_resource_router_mode.cpp
    )

add_executable(captived ${SERVER_SRCS})

target_include_directories(captived
    PUBLIC include
    PUBLIC ${libevent_INCLUDE_DIRS}
    PUBLIC ${JANSSON_INCLUDE_DIRS}
    )

target_link_libraries(captived
    PUBLIC ${libevent_LIBRARIES}
    PUBLIC ${JANSSON_LIBRARIES}
    )

set_target_properties(captived PROPERTIES
    OUTPUT_NAME captived
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    )

install(
    TARGETS captived
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
