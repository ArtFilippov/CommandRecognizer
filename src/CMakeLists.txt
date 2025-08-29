file(GLOB headers *.h)
file(GLOB sources *.cpp)


add_library(patterns ${sources} ${headers})

target_link_libraries(patterns PUBLIC common)

target_include_directories(patterns PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:${include_dest}>)

install(TARGETS patterns EXPORT patterns DESTINATION ${main_lib_dest})
install(FILES ${headers} DESTINATION ${include_dest})
