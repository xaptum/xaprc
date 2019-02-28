cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
set(INSTALL_SYSTEMDDIR "/lib/systemd/system")

install(
  FILES
  ${CMAKE_CURRENT_LIST_DIR}/captived.service
  DESTINATION
  ${INSTALL_SYSTEMDDIR}
)
