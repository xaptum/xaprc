cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

project(captived
  VERSION "0.1.6.1"
  )

include(GNUInstallDirs)
include(CTest)

option(BUILD_SHARED_LIBS "Build with shared libraries" ON)
option(BUILD_SYSTEMD "Build with systemd support" ON)
option(CHECK_FORMAT "Check source format during build" ON)

find_package(libevent REQUIRED QUIET)
find_package(jansson REQUIRED QUIET)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

add_custom_target(check-format
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/format.sh check || (exit 0)
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

set(SERVER_SRCS
    src/main.cpp
    src/http/server.cpp
    src/rest/aggregate_resource.cpp
    src/rest/firmware.cpp
    src/rest/model.cpp
    src/rest/line_resource.cpp
    src/rest/mode.cpp
    src/rest/reboot.cpp
    src/rest/uptime.cpp
    src/rest/wifi_config.cpp
    src/rest/wifi_status.cpp
    )

add_executable(captived ${SERVER_SRCS})

target_include_directories(captived
    PUBLIC src
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

if (BUILD_SYSTEMD)
  add_subdirectory(systemd)
endif()

if (BUILD_TESTING)
  add_subdirectory(integration-tests)
endif()

if (CHECK_FORMAT)
  add_dependencies(captived check-format)
endif()
