cmake_minimum_required(VERSION 3.4)
project(conflagrant)

#####################################################
### force C++11 compliant compiler w/o extensions ###
#####################################################

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#########################################
### set CMake custom module directory ###
#########################################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

###################################
### setup external dependencies ###
###################################

function(use_package name found_variable include_dirs_name library_name)
    find_package(${name})
    if (${found_variable})
        message(STATUS "${name} found.")
    else (${found_variable})
        message(FATAL_ERROR "${name} not found.")
    endif (${found_variable})

    if (${library_name})
        set(EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES})
    endif (${library_name})
    if (${include_dirs_name})
        set(EXTERNAL_INCLUDE_DIRS ${EXTERNAL_INCLUDE_DIRS} ${include_dirs_name})
    else (${include_dirs_name})
        message(AUTHOR_WARNING "use_package(${name}) called with empty 'include_dirs_name' string")
    endif (${include_dirs_name})
endfunction()

use_package(OpenGL OPENGL_FOUND OPENGL_INCLUDE_DIR OPENGL_LIBRARIES)
use_package(GLM GLM_FOUND GLM_INCLUDE_DIRS "")
if (NOT APPLE)
    use_package(GLEW GLEW_FOUND GLEW_INCLUDE_DIRS GLEW_LIBRARIES)
endif ()
use_package(OpenCL OpenCL_FOUND OpenCL_INCLUDE_DIRS OpenCL_LIBRARIES)

#############
#### GLM ####
#############

include_directories(${EXTERNAL_INCLUDE_DIRS})

################################################
### gather all source files and header files ###
### export them to parent scope              ###
################################################

file(GLOB_RECURSE HH_FILES "src/*.hh")
file(GLOB_RECURSE CC_FILES "src/*.cc")

#######################################################################
### decide if creating a library target, and if so shared or static ###
#######################################################################

set(BUILD_SHARED_LIBRARY OFF CACHE BOOL "Build conflagrant-core as a shared library (otherwise static), if BUILD_LIBRARY is ON")

if (BUILD_SHARED_LIBRARY)
    add_library(conflagrant SHARED ${HH_FILES} ${CC_FILES})
else ()
    add_library(conflagrant STATIC ${CC_FILES} ${CC_FILES})
endif ()

target_link_libraries(conflagrant ${EXTERNAL_LIBRARIES})
set(CONFLAGRANT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CONFLAGRANT_LIBRARIES conflagrant)

######################
### build examples ###
######################

add_subdirectory(examples)
