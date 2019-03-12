cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(wpa-credentials)

set(WPA_FOLDER_NAME "connman" CACHE PATH "The name of the directory to install to")
set(WPA_INSTALL_PATH "/data/${WPA_FOLDER_NAME}" CACHE PATH "The full path to store WPA credentials")
set(WPA_SYMLINK_DEST "/etc" CACHE PATH "Path where symlink to the data install path should go")

#Use default SSID and password if neither are set
if("${XAPRC_SSID}" STREQUAL "" AND "${XAPRC_PASS}" STREQUAL "") 
	set(XAPRC_SSID "xaprc_default")
	set(XAPRC_PASS "password123!")
	message("NOTICE: No wifi credentials provided, using default SSID=" ${XAPRC_SSID} " and PASS=" ${XAPRC_PASS})
endif()

#Configure and create WPA credential conf file
configure_file(wifi.config.in wifi.config @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/wifi.config
        DESTINATION ${WPA_INSTALL_PATH}
)

#Create the symlink to the install directory
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${WPA_INSTALL_PATH}
    ${WPA_FOLDER_NAME} 
)

#Move the created symlink into the dest directory
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${WPA_FOLDER_NAME}
	DESTINATION ${WPA_SYMLINK_DEST}
)
