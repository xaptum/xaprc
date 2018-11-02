cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

configure_file(xbridge.service.in xbridge.service @ONLY)
configure_file(xbridge-setup.service.in xbridge-setup.service @ONLY)

set(INSTALL_SYSTEMDDIR "/lib/systemd/system")

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/xbridge.service
        ${CMAKE_CURRENT_BINARY_DIR}/xbridge-setup.service
  DESTINATION ${INSTALL_SYSTEMDDIR}
)
