file(GLOB headers *.h)
file(GLOB sources *.cpp)


add_library(commandRecognizer ${sources} ${headers})

target_link_libraries(commandRecognizer PUBLIC common)

target_include_directories(commandRecognizer PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:${include_dest}>)

install(TARGETS commandRecognizer EXPORT commandRecognizer DESTINATION ${main_lib_dest})
install(FILES ${headers} DESTINATION ${include_dest})
