cmake_minimum_required(VERSION 3.29.0)

set(CMAKE_SYSTEM_NAME Generic)

include(${CMAKE_PREFIX_PATH}/toolchain.cmake)

include_directories(${CMAKE_PREFIX_PATH})

find_package(common)

project(PATTERNS CXX)

set(include_dest "patterns")
set(main_lib_dest "patterns/lib/patterns")


install(FILES patterns-config.cmake DESTINATION ${main_lib_dest})
install(EXPORT patterns DESTINATION "${main_lib_dest}")

add_subdirectory(src)