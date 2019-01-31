cmake_minimum_required (VERSION 2.6)
project (MQTT)

set (MQTT_VERSION_MAJOR 1)
set (MQTT_VERSION_MINOR 0)
set (MQTT_SRC_LIST MQTT_Main.cpp MQTT_Source.cpp)

configure_file (
  "${PROJECT_SOURCE_DIR}/MQTT_Config.h.in"
  "${PROJECT_BINARY_DIR}/MQTT_Config.h"
  )

include_directories ("${PROJECT_BINARY_DIR}")

add_executable (MQTT ${TCP_SRC_LIST})

install (TARGETS MQTT DESTINATION bin)
install (FILES Initialization.ini DESTINATION bin)

include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VERSION_MAJOR "${MQTT_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${MQTT_VERSION_MINOR}")
include (CPack)