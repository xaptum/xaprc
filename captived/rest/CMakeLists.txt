cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(REST_SRCS
  src/get_file.cpp
  src/mode_get_put.cpp
  src/put_file.cpp
  src/reboot.cpp
  src/root_resource.cpp
  src/wifi_config.cpp
  )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")


add_library(rest ${REST_SRCS})

target_include_directories(rest
  PUBLIC ${captived_INCLUDE_DIRS}
  PUBLIC ${libevent_INCLUDE_DIRS}
  PUBLIC ${JANSSON_INCLUDE_DIRS}
   )

target_link_libraries(rest
   PUBLIC ${libevent_LIBRARIES}
   PUBLIC ${JANSSON_LIBRARIES}
   )

set_target_properties(rest PROPERTIES
  OUTPUT_NAME rest
  FOLDER rest
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  )

install(TARGETS rest LIBRARY DESTINATION lib)
