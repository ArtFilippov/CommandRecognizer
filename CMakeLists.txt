cmake_minimum_required(VERSION 3.29.0)

set(CMAKE_SYSTEM_NAME Generic)

include(${CMAKE_PREFIX_PATH}/toolchain.cmake)

include_directories(${CMAKE_PREFIX_PATH})

find_package(common)

project(COMMAND_RECOGNIZER CXX)

set(include_dest "commandRecognizer")
set(main_lib_dest "commandRecognizer/lib/commandRecognizer")


install(FILES commandRecognizer-config.cmake DESTINATION ${main_lib_dest})
install(EXPORT commandRecognizer DESTINATION "${main_lib_dest}")

add_subdirectory(src)