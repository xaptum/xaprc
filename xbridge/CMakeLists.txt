cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project(xbridge
  VERSION 0.1.0
  LANGUAGES C
)

option(BUILD_SYSTEMD "Build with systemd support" ON)

include(GNUInstallDirs)

find_package(PCAP)

add_executable(xbridge
  src/xbridge.c
)

target_link_libraries(xbridge
  PRIVATE
  pcap
)

set_target_properties(xbridge
  PROPERTIES
  C_STANDARD 99
)

install(
  TARGETS xbridge
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (BUILD_SYSTEMD)
# add_subdirectory(systemd)
endif()
