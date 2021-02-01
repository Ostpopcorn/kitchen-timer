# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/main/src/*.*)

idf_component_register(SRCS ${app_sources}
                       INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/main/include/ ${CMAKE_SOURCE_DIR}/main/include/screen_view_16x2
                       )
