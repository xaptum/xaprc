cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake-modules)
include(InstallSymlink)
include(GNUInstallDirs)

project(enftun-data)

configure_file(enftun-data@.service.in enftun-data@.service @ONLY)
configure_file(enf0.conf ${CMAKE_CURRENT_BINARY_DIR}/enf0.conf COPYONLY)
configure_file(wait_for_iface ${CMAKE_CURRENT_BINARY_DIR}/wait_for_iface COPYONLY)
configure_file(wait_for_iface_up ${CMAKE_CURRENT_BINARY_DIR}/wait_for_iface_up COPYONLY)

set(INSTALL_SYSTEMDDIR "/lib/systemd/system")
set(INSTALL_SYSTEMDCONF "/etc/systemd/system")
set(INSTALL_TARGET "secure-host.target.wants")
set(INSTALL_ENFCONF "/etc/enftun")

install(
  FILES
  ${CMAKE_CURRENT_BINARY_DIR}/enftun-data@.service
  DESTINATION
  ${INSTALL_SYSTEMDDIR}
)

install(
  FILES
  ${CMAKE_CURRENT_BINARY_DIR}/enf0.conf
  DESTINATION
  ${INSTALL_ENFCONF}
)

install(
  PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/wait_for_iface ${CMAKE_CURRENT_BINARY_DIR}/wait_for_iface_up
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install_symlink(${INSTALL_SYSTEMDDIR}/enftun-data@.service ${INSTALL_SYSTEMDCONF}/${INSTALL_TARGET}/enftun-data@enf0.service)