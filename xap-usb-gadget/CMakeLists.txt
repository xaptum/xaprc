cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

add_subdirectory(systemd)
