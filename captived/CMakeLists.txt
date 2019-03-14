cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(GNUInstallDirs)

project(captived
  VERSION "0.1.1"
  )


option(BUILD_SHARED_LIBS "Build with shared libraries" ON)
option(BUILD_SYSTEMD "Build with systemd support" ON)

enable_testing()

set(captived_INCLUDE_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${CMAKE_CURRENT_LIST_DIR}/json/include"
    "${CMAKE_CURRENT_LIST_DIR}/http_server/include"
    "${CMAKE_CURRENT_LIST_DIR}/rest/include"
    )

find_package(libevent REQUIRED QUIET)
find_package(jansson REQUIRED QUIET)

add_subdirectory(http_server)
add_subdirectory(rest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(SERVER_SRCS
    src/main.cpp
    )

add_executable(captived ${SERVER_SRCS})

target_include_directories(captived
    PUBLIC ${captived_INCLUDE_DIRS}
    PUBLIC ${libevent_INCLUDE_DIRS}
    PUBLIC ${JANSSON_INCLUDE_DIRS}
    )

target_link_libraries(captived
    PRIVATE http_server
    PRIVATE rest
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

if (BUILD_SYSTEMD)
	add_subdirectory(systemd)
endif()

add_subdirectory(integration-tests)
