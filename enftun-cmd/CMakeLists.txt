cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake-modules)
include(InstallSymlink)
include(GNUInstallDirs)

project(enftun-cmd)

configure_file(enf1.conf ${CMAKE_CURRENT_BINARY_DIR}/enf1.conf COPYONLY)

set(INSTALL_SYSTEMDCONF "/etc/systemd/system")
set(INSTALL_TARGET "multi-user.target.wants")
set(INSTALL_ENFCONF "/etc/enftun")
set(INSTALL_SYSTEMDDIR "/lib/systemd/system/")

install(
  FILES
  ${CMAKE_CURRENT_BINARY_DIR}/enf1.conf
  DESTINATION
  ${INSTALL_ENFCONF}
)

install_symlink(${INSTALL_SYSTEMDDIR}/enftun@.service ${INSTALL_SYSTEMDCONF}/${INSTALL_TARGET}/enftun@enf1.service)