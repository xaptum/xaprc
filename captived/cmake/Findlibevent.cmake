# Find the libevent libary

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(libevent_PKGCONF libevent)

# Find the include dirs
find_path(libevent_INCLUDE_DIR
  NAMES event2/event.h
  PATHS ${libevent_PKGCONF_INCLUDE_DIRS}
  )

find_library(libevent_LIBRARY
  NAMES event
  PATHS ${libevent_PKGCONFIG_LIBRARY_DIRS}
  )

set(libevent_PROCESS_INCLUDES libevent_INCLUDE_DIR)
set(libevent_PROCESS_LIBS libevent_LIBRARY)

libfind_process(libevent)
