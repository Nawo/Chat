# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ClientUI_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ClientUI_autogen.dir\\ParseCache.txt"
  "ClientUI_autogen"
  )
endif()
