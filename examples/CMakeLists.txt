cmake_minimum_required(VERSION 3.4)
project(conflagrant-examples)

#####################################################
### force C++11 compliant compiler w/o extensions ###
#####################################################

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

######################
### build examples ###
######################

set(examples
        example1
        example2)

foreach (example ${examples})
    if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${example}")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${example})
    else ()
        include_directories(${CFL_INCLUDE_DIRS})
        add_executable(${example} ${example}.cc)
        target_link_libraries(${example} ${CFL_LIBRARIES})
    endif ()
endforeach ()
