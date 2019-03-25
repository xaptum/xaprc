cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(macset NONE)

include(GNUInstallDirs)

option(BUILD_SYSTEMD "Build with systemd support" ON)
option(BUILD_WLAN0 "Build with the default wlan0 config" ON)

install(PROGRAMS ${CMAKE_CURRENT_LIST_DIR}/macset
        DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (BUILD_SYSTEMD)
  add_subdirectory(systemd)
endif()

if (BUILD_WLAN0)
  add_subdirectory(wlan0)
endif()
