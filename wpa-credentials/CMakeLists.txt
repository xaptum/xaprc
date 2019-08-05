cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(wpa-credentials)

set(WPA_INSTALL_PATH "/data/connman" CACHE PATH "The full path to store WPA credentials")
set(WPA_RECOVERY_INSTALL_PATH "/etc/connman" CACHE PATH "The full path to store recovery WPA credentials")

#Use default SSID and password if neither are set
if("${XAPRC_SSID}" STREQUAL "" AND "${XAPRC_PASS}" STREQUAL "")
	set(XAPRC_SSID "xaprc_default")
	set(XAPRC_PASS "password123!")
	message("NOTICE: No wifi credentials provided, using default SSID=" ${XAPRC_SSID} " and PASS=" ${XAPRC_PASS})
endif()

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

install(FILES ${CMAKE_CURRENT_LIST_DIR}/passthrough-recovery.config
        DESTINATION ${WPA_RECOVERY_INSTALL_PATH}/passthrough
	RENAME recovery.config
)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/secure-host-recovery.config
        DESTINATION ${WPA_RECOVERY_INSTALL_PATH}/secure-host
	RENAME recovery.config
)
