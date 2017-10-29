cmake_minimum_required(VERSION 3.8.2)
project(conflagrant)

#####################################################
### force C++14 compliant compiler w/o extensions ###
#####################################################

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

################################
### pass build type variable ###
################################

if ((${CMAKE_BUILD_TYPE} STREQUAL Debug) OR (${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo))
    message(STATUS "Build type: Debug")
    set(CFL_DEBUG 1)
elseif (${CMAKE_BUILD_TYPE} STREQUAL Release)
    message(STATUS "Build type: Release")
    set(CFL_DEBUG 0)
else ()
    message(AUTHOR_WARNING "Could not make out build type.")
endif ()
add_definitions(-DCFL_DEBUG=${CFL_DEBUG})

#########################################
### set CMake custom module directory ###
#########################################

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

###################################
### setup external dependencies ###
###################################

macro(cfl_use_include_dir include_dir)
    set(EXTERNAL_INCLUDE_DIRS ${EXTERNAL_INCLUDE_DIRS} ${include_dir})
endmacro()

macro(cfl_use_library library_name)
    set(EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES} ${library_name})
endmacro()

macro(cfl_use_package name found_variable include_dir library_name)
    find_package(${name})
    if (${${found_variable}})
        message(STATUS "${name} found.")
    else ()
        message(FATAL_ERROR "${name} not found.")
    endif ()

    if (${library_name})
        cfl_use_library(${${library_name}})
    endif ()

    if (NOT "${include_dir}" STREQUAL "")
        cfl_use_include_dir(${${include_dir}})
    else ()
        message(STATUS "MAYBE AN ERROR: cfl_use_package(${name}) called with empty 'include_dir' string")
    endif ()
endmacro()

cfl_use_package(GLFW3 GLFW3_FOUND GLFW3_INCLUDE_DIR GLFW3_LIBRARY)

cfl_use_package(OpenGL OPENGL_FOUND OPENGL_INCLUDE_DIR "")
cfl_use_library(${OPENGL_gl_LIBRARY})
cfl_use_library(${OPENGL_glu_LIBRARY})

cfl_use_package(GLM GLM_FOUND GLM_INCLUDE_DIRS "")
if (NOT APPLE)
    cfl_use_package(GLEW GLEW_FOUND GLEW_INCLUDE_DIRS GLEW_LIBRARIES)
endif ()
cfl_use_package(OpenCL OpenCL_FOUND OpenCL_INCLUDE_DIRS OpenCL_LIBRARIES)
cfl_use_package(Assimp ASSIMP_FOUND assimp_INCLUDE_DIRS ASSIMP_LIBRARIES)
cfl_use_package(JsonCpp JSONCPP_FOUND JSONCPP_INCLUDE_DIRS JSONCPP_LIBRARY)

include_directories(${EXTERNAL_INCLUDE_DIRS})
foreach (dir ${EXTERNAL_INCLUDE_DIRS})
    message(STATUS ${dir})
endforeach ()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(external)

################################
### configure Conflagrant    ###
### set base asset directory ###
################################

include(cmake/EcsConfig.cmake)

################################################
### gather all source files and header files ###
### export them to parent scope              ###
################################################

set(HH_FILES
        src/conflagrant/conflagrant.hh
        src/conflagrant/types.hh
        src/conflagrant/GL.hh
        src/conflagrant/CL.hh
        src/conflagrant/assets/Asset.hh
        src/conflagrant/assets/AssetLoader.hh
        src/conflagrant/assets/AssetManager.hh
        src/conflagrant/assets/Image.hh
        src/conflagrant/assets/loaders/MeshLoader.hh
        src/conflagrant/components/MeshComponent.hh
        src/conflagrant/ecs/Component.hh
        src/conflagrant/ecs/ComponentFilter.hh
        src/conflagrant/ecs/EcsManager.hh
        src/conflagrant/ecs/EcsTypes.hh
        src/conflagrant/ecs/System.hh
        src/conflagrant/ecs/internal/ClassType.hh
        src/conflagrant/ecs/internal/ComponentType.hh
        src/conflagrant/ecs/internal/EcsAssert.hh
        src/conflagrant/ecs/internal/EntityStorage.hh
        src/conflagrant/ecs/internal/InternalEcsTypes.hh
        src/conflagrant/ecs/internal/SystemType.hh
        src/conflagrant/geometry/Mesh.hh
        src/conflagrant/serialization/ReadStream.hh
        src/conflagrant/serialization/Serializable.hh
        src/conflagrant/serialization/WriteStream.hh
        )
set(CC_FILES
        src/conflagrant/assets/AssetManager.cc
        src/conflagrant/conflagrant.cc
        src/conflagrant/TestGLFW3.cc
        src/conflagrant/CL.cc
        src/conflagrant/GL.cc
        src/conflagrant/ecs/EcsManager.cc
        src/conflagrant/ecs/Entity.cc
        src/conflagrant/ecs/System.cc
        src/conflagrant/ecs/internal/ComponentType.cc
        src/conflagrant/ecs/internal/EntityStorage.cc
        )

#######################################################################
### decide if creating a library target, and if so shared or static ###
#######################################################################

set(BUILD_SHARED_LIBRARY OFF CACHE BOOL "Build conflagrant-core as a shared library (otherwise static), if BUILD_LIBRARY is ON")

if (BUILD_SHARED_LIBRARY)
    add_library(conflagrant SHARED ${HH_FILES} ${CC_FILES})
else ()
    add_library(conflagrant STATIC ${HH_FILES} ${CC_FILES})
endif ()

target_link_libraries(conflagrant ${EXTERNAL_LIBRARIES})
set(CFL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CFL_LIBRARIES conflagrant)

################################
### build examples and tests ###
################################

add_subdirectory(examples)
add_subdirectory(test)
