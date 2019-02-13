cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(mender-identity)

set(MENDER_INSTALL_PATH "/usr/share/mender/identity" CACHE PATH "default install path")

install(
  PROGRAMS
  mender-device-identity
  DESTINATION
  ${MENDER_INSTALL_PATH}
  )
