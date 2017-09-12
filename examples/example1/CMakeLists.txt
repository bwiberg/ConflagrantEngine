include_directories(${CONFLAGRANT_INCLUDE_DIRS})
add_executable(example1 example1.cc)
target_link_libraries(example1 ${CONFLAGRANT_LIBRARIES})
