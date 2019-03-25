cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(INSTALL_ETCDIR "/etc/macset")

install(
  FILES
  ${CMAKE_CURRENT_LIST_DIR}/wlan0.conf
  DESTINATION
  ${INSTALL_ETCDIR}
  )
