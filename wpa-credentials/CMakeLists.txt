cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake-modules)
include(InstallSymlink)

project(wpa-credentials)

set(CONNMAN_CONF_PATH "/etc/connman" CACHE PATH "The full path to store connman config files")
set(WPA_INSTALL_PATH "/data/connman" CACHE PATH "The full path to store WPA credentials")
set(WPA_RECOVERY_INSTALL_PATH "/etc/connman" CACHE PATH "The full path to store recovery WPA credentials")

#Use default SSID and password if neither are set
if("${XAPRC_SSID}" STREQUAL "" AND "${XAPRC_PASS}" STREQUAL "")
	set(XAPRC_SSID "xaprc_default")
	set(XAPRC_PASS "password123!")
	message("NOTICE: No wifi credentials provided, using default SSID=" ${XAPRC_SSID} " and PASS=" ${XAPRC_PASS})
endif()

# Install connman conf file
install(FILES ${CMAKE_CURRENT_LIST_DIR}/main.conf
        DESTINATION ${CONNMAN_CONF_PATH}
)

#Configure and create WPA credential conf files
configure_file(passthrough-wifi.config.in passthrough-wifi.config @ONLY)
configure_file(secure-host-wifi.config.in secure-host-wifi.config @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/passthrough-wifi.config
        DESTINATION ${WPA_INSTALL_PATH}/passthrough
	RENAME wifi.config
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/secure-host-wifi.config
        DESTINATION ${WPA_INSTALL_PATH}/secure-host
	RENAME wifi.config
)

install_symlink(${WPA_INSTALL_PATH}/secure-host ${WPA_INSTALL_PATH}/secure-lan)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/passthrough-recovery.config
        DESTINATION ${WPA_RECOVERY_INSTALL_PATH}/passthrough
	RENAME recovery.config
)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/secure-host-recovery.config
        DESTINATION ${WPA_RECOVERY_INSTALL_PATH}/secure-host
	RENAME recovery.config
)

install_symlink(${WPA_RECOVERY_INSTALL_PATH}/secure-host ${WPA_RECOVERY_INSTALL_PATH}/secure-lan)
