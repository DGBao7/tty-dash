# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  [[CMakeFiles\TtyDashboard_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\TtyDashboard_autogen.dir\ParseCache.txt]]
  "TtyDashboard_autogen"
  )
endif()
