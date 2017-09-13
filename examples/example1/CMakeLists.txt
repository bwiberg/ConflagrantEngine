include_directories(${CFL_INCLUDE_DIRS})
add_definitions(-DEXAMPLE1_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")
add_executable(example1 example1.cc)
target_link_libraries(example1 ${CFL_LIBRARIES})
