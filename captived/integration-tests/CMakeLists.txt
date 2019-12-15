cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(CURRENT_TEST_BINARY_DIR ${CMAKE_BINARY_DIR}/testBin/integration-tests)

# Find the Python and virtualenv programs.
find_package(PythonInterp 3 REQUIRED)

set(TEST_FILES
  firmware_Test.py
  mode_Test.py
  reboot_Test.py
  root_level_status_Test.py
  secure_lan_symlink_Test.py
  simple_get_Test.py
  uptime_Test.py
  wifi_Test.py
  wifi_config_Test.py
  wifi_status_Test.py
  )

add_custom_target(integration-tests ALL)

# Create the test directory and make it a virtualenv.
add_custom_command(
  TARGET integration-tests POST_BUILD
  COMMAND ${PYTHON_EXECUTABLE} -m venv ${CURRENT_TEST_BINARY_DIR}
  )

# Install required packages
add_custom_command(
  TARGET integration-tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt ${CURRENT_TEST_BINARY_DIR}/requirements.txt
  COMMAND ${CURRENT_TEST_BINARY_DIR}/bin/pip install -r ${CURRENT_TEST_BINARY_DIR}/requirements.txt --upgrade
  )

# Install the local Xaptum package
add_custom_command(TARGET integration-tests POST_BUILD
  COMMAND cmake -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/xaptum ${CURRENT_TEST_BINARY_DIR}/xaptum
  )

# Install each test executable
foreach(test_file ${TEST_FILES})
  # copy each test executable into the test subdirectory
  add_custom_command(TARGET integration-tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${test_file} ${CURRENT_TEST_BINARY_DIR}/${test_file}
    )

  # register each test executable with CTest
  add_test(NAME captived_integration_tests_${test_file}
    COMMAND ${CURRENT_TEST_BINARY_DIR}/bin/python ${CURRENT_TEST_BINARY_DIR}/${test_file}
    WORKING_DIRECTORY ${CURRENT_TEST_BINARY_DIR}
    )
endforeach()

# Copy the test config directory
add_custom_command(TARGET integration-tests POST_BUILD
  # using cp because copy_directory does not preserve symbolic links
  COMMAND cp -P -R ${CMAKE_CURRENT_SOURCE_DIR}/config ${CURRENT_TEST_BINARY_DIR}/config
  )

