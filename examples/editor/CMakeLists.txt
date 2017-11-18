include_directories(${CFL_INCLUDE_DIRS})
add_executable(example_editor example_editor.cc)
target_link_libraries(example_editor ${CFL_LIBRARIES})
