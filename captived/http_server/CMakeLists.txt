cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(HTTP_SERVER_SRCS
  src/http_server.cpp
  )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")


add_library(http_server ${HTTP_SERVER_SRCS})

target_include_directories(http_server
  PUBLIC ${captived_INCLUDE_DIRS}
  PUBLIC ${libevent_INCLUDE_DIRS}
  PUBLIC ${JANSSON_INCLUDE_DIRS}
  )

target_link_libraries(http_server
   PUBLIC ${libevent_LIBRARIES}
   PUBLIC ${JANSSON_LIBRARIES}
   )

set_target_properties(http_server PROPERTIES
  OUTPUT_NAME http_server
  FOLDER http_server
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  )

install(TARGETS http_server LIBRARY DESTINATION lib)
