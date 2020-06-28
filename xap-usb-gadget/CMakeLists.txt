cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(GNUInstallDirs)


option(ENABLE_HSS "Enable support for Host Socket Sharing" OFF)
option(ENABLE_SERIAL_CONSOLE "Expose a serial console via the USB device" OFF)

add_executable(xap-usb-gadget
	xap-usb-gadget.c)

target_compile_definitions(xap-usb-gadget
PUBLIC -DXAP_PRODUCT_ID=${XAP_PRODUCT_ID}
       -DXAP_PRODUCT_STR=\"${XAP_PRODUCT_STR}\"
)

if (ENABLE_HSS)
  target_compile_definitions(xap-usb-gadget
  PUBLIC -DXAP_HSS=y
  )
endif()

if (ENABLE_SERIAL_CONSOLE)
  target_compile_definitions(xap-usb-gadget 
  PUBLIC -DXAP_SERIAL=y
  )
endif()

install(
    TARGETS xap-usb-gadget
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

target_link_libraries(xap-usb-gadget 
PUBLIC config 
       usbgx
)

add_subdirectory(systemd)
