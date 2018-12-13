cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

configure_file(xbridge.service.in xbridge.service @ONLY)
configure_file(xbridge-setup.service.in xbridge-setup.service @ONLY)

configure_file(xbridges.service.in xbridges.service @ONLY)
configure_file(xbridges-setup.service.in xbridges-setup.service @ONLY)

set(INSTALL_SYSTEMDDIR "/lib/systemd/system")

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/xbridge.service
        ${CMAKE_CURRENT_BINARY_DIR}/xbridge-setup.service
        ${CMAKE_CURRENT_BINARY_DIR}/xbridges.service
        ${CMAKE_CURRENT_BINARY_DIR}/xbridges-setup.service
  DESTINATION ${INSTALL_SYSTEMDDIR}
)
