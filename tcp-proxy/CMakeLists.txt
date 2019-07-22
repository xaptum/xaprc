cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

add_executable(psock_test
	psock_test.c)

install(
    TARGETS psock_test
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
