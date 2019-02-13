cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(etc-hosts)

set(ETC_HOSTS_INSTALL_PATH "/etc" CACHE PATH "Default install path")
set(ETC_HOSTS_ENV "dev" CACHE STRING "Install hosts file for the prod or dev environment.")

if(ETC_HOSTS_ENV STREQUAL "dev")
  set(MENDER_XAPTUM_XYZ_ADDRESS "2607:8f80:8100::07a:1")
elseif(ETC_HOSTS_ENV STREQUAL  "prod")
  set(MENDER_XAPTUM_XYZ_ADDRESS "2607:8f80:80c0::07a:1")
endif()

configure_file(hosts.in hosts @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hosts
        DESTINATION ${ETC_HOSTS_INSTALL_PATH}
)
