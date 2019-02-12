cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(mender-certs)

set(MENDER_INSTALL_PATH "/etc/mender" CACHE PATH "default install path")
set(MENDER_ENV "dev" CACHE STRING "Install certs for the prod or dev environment.")

install(
  FILES
  ${MENDER_ENV}/Xaptum-Mender.crt
  ${MENDER_ENV}/xaptum-mender.signing.pub.pem
  DESTINATION
  ${MENDER_INSTALL_PATH}
  )
