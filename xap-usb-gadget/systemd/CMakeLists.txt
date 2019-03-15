cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

configure_file(xap-usb-gadget.service.in xap-usb-gadget.service @ONLY)

set(INSTALL_SYSTEMDDIR "/lib/systemd/system")

install(
  FILES
        ${CMAKE_CURRENT_BINARY_DIR}/xap-usb-gadget.service
  DESTINATION ${INSTALL_SYSTEMDDIR}
)
