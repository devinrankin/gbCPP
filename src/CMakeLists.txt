cmake_minimum_required(VERSION 4.2)
project(gbCPP)

set(UTIL_DIR "${CMAKE_SOURCE_DIR}/utils")

add_library(utils)
target_sources(utils
  PRIVATE ${UTIL_DIR}/files.cc

  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      ${UTIL_DIR}
    FILES
      ${UTIL_DIR}/files.h
      ${UTIL_DIR}/types.h
)

add_executable(gbCPP)
target_sources(gbCPP
  PRIVATE main.cc
)
target_link_libraries(gbCPP utils)

set_property(TARGET gbCPP PROPERTY CXX_STANDARD 26)
