####################################################################################################################
## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ##
#                                                                                                                 ##
##    e88~-_    ,88~-_   888b    | 888~~  888          e       e88~~\  888~-_        e      888b    | ~~~888~~~    #
#    d888   \  d888   \  |Y88b   | 888___ 888         d8b     d888     888   \      d8b     |Y88b   |    888      ##
##   8888     88888    | | Y88b  | 888    888        /Y88b    8888 __  888    |    /Y88b    | Y88b  |    888       #
#    8888     88888    | |  Y88b | 888    888       /  Y88b   8888   | 888   /    /  Y88b   |  Y88b |    888      ##
##   Y888   /  Y888   /  |   Y88b| 888    888      /____Y88b  Y888   | 888_-~    /____Y88b  |   Y88b|    888       #
#     "88_-~    `88_-~   |    Y888 888    888____ /      Y88b  "88__/  888 ~-_  /      Y88b |    Y888    888      ##
##                                                                                                                 #
## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ##
####################################################################################################################

cmake_minimum_required(VERSION 3.8.2)
project(conflagrant)

#####################################################
### force C++14 compliant compiler w/o extensions ###
#####################################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

################################
### pass build type variable ###
################################

# add_definitions(-DFORCE_USE_DOLLAR)

if ((${CMAKE_BUILD_TYPE} STREQUAL Debug) OR (${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo))
    message(STATUS "Build type: Debug")
    add_definitions(-DCFL_DEBUG)
elseif (${CMAKE_BUILD_TYPE} STREQUAL Release)
    message(STATUS "Build type: Release")
else ()
    message(AUTHOR_WARNING "Could not make out build type.")
endif ()

add_definitions(-DENABLE_VOXEL_CONE_TRACING)

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
cfl_use_package(GLEW GLEW_FOUND GLEW_INCLUDE_DIRS GLEW_LIBRARIES)
cfl_use_package(OpenCL OpenCL_FOUND OpenCL_INCLUDE_DIRS OpenCL_LIBRARIES)
cfl_use_package(Assimp ASSIMP_FOUND assimp_INCLUDE_DIRS assimp_LIBRARIES)
cfl_use_package(JsonCpp JSONCPP_FOUND JSONCPP_INCLUDE_DIRS JSONCPP_LIBRARY)

add_subdirectory(external/entityx)
cfl_use_library(entityx)
cfl_use_include_dir(${CMAKE_CURRENT_SOURCE_DIR}/external/entityx)

add_subdirectory(external/crossguid)
cfl_use_library(xg)

add_subdirectory(external/imgui)
cfl_use_include_dir(external/imgui)
cfl_use_library(imgui)

cfl_use_include_dir(external)

include_directories(${EXTERNAL_INCLUDE_DIRS})
foreach (dir ${EXTERNAL_INCLUDE_DIRS})
    message(STATUS ${dir})
endforeach ()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

################################
### configure Conflagrant    ###
### set base asset directory ###
################################

set(CFL_BUILTIN_SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/conflagrant/shaders)
set(CFL_BUILTIN_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
add_definitions(-DBUILTIN_SHADER_DIR="${CFL_BUILTIN_SHADER_DIR}")
add_definitions(-DBUILTIN_ASSETS_DIR="${CFL_BUILTIN_ASSETS_DIR}")

################################################
### gather all source files and header files ###
### export them to parent scope              ###
################################################

set(HH_FILES
        src/conflagrant/homedirectory.hh
        src/conflagrant/args.hh
        src/conflagrant/conflagrant.hh
        src/conflagrant/logging.hh
        src/conflagrant/math.hh
        src/conflagrant/Engine.hh
        src/conflagrant/Time.hh
        src/conflagrant/Timer.hh
        src/conflagrant/ComponentFactory.hh
        src/conflagrant/SystemFactory.hh
        src/conflagrant/types.hh
        src/conflagrant/geometry.hh
        src/conflagrant/events.hh
        src/conflagrant/GL.hh
        src/conflagrant/CL.hh
        src/conflagrant/Window.hh
        src/conflagrant/factory_util.hh
        src/conflagrant/RenderStats.hh
        src/conflagrant/ShaderSourceManager.hh
        src/conflagrant/SmartValue.hh
        src/conflagrant/assets/Asset.hh
        src/conflagrant/assets/AssetLoader.hh
        src/conflagrant/assets/AssetManager.hh
        src/conflagrant/assets/Texture.hh
        src/conflagrant/assets/loaders/ModelLoader.hh
        src/conflagrant/assets/loaders/TextureLoader.hh
        src/conflagrant/components/ActiveCamera.hh
        src/conflagrant/components/VelocityAnimation.hh
        src/conflagrant/components/Mesh.hh
        src/conflagrant/components/Name.hh
        src/conflagrant/components/Guid.hh
        src/conflagrant/components/Model.hh
        src/conflagrant/components/Transform.hh
        src/conflagrant/components/PerspectiveCamera.hh
        src/conflagrant/components/OrthographicCamera.hh
        src/conflagrant/components/DirectionalLight.hh
        src/conflagrant/components/DirectionalLightShadow.hh
        src/conflagrant/components/DirectionalLightAnimation.hh
        src/conflagrant/components/PointLight.hh
        src/conflagrant/components/PeriodicalAnimation.hh
        src/conflagrant/components/Skydome.hh
        src/conflagrant/components/BoundingSphere.hh
        src/conflagrant/components/VctProperties.hh
        src/conflagrant/components/SnowEmitter.hh
        src/conflagrant/System.hh
        src/conflagrant/systems/Animator.hh
        src/conflagrant/systems/CameraController.hh
        src/conflagrant/systems/EcsDebugger.hh
        src/conflagrant/systems/DeferredRenderer.hh
        src/conflagrant/systems/ForwardRenderer.hh
        src/conflagrant/systems/SnowfallAnimator.hh
        src/conflagrant/systems/system_util.hh
        src/conflagrant/assets/Mesh.hh
        src/conflagrant/assets/Model.hh
        src/conflagrant/assets/Material.hh
        src/conflagrant/gl/GlObject.hh
        src/conflagrant/gl/State.hh
        src/conflagrant/assets/loaders/AssimpGlmConvert.hh
        src/conflagrant/gl/GlObject.hh
        src/conflagrant/gl/Buffer.hh
        src/conflagrant/gl/Framebuffer.hh
        src/conflagrant/gl/Renderbuffer.hh
        src/conflagrant/gl/Texture.hh
        src/conflagrant/gl/Mesh.hh
        src/conflagrant/gl/VertexArray.hh
        src/conflagrant/gl/Shader.hh
        src/conflagrant/gl/DoubleBufferedTexture1D.hh
        src/conflagrant/gl/DoubleBufferedTexture2D.hh
        src/conflagrant/cl/Environment.hh
        src/conflagrant/serialization/serialize.hh
        src/conflagrant/serialization/array.hh
        src/conflagrant/serialization/vector.hh
        src/conflagrant/serialization/crossguid.hh
        src/conflagrant/serialization/glm.hh
        src/conflagrant/serialization/path.hh
        src/conflagrant/input.hh
        src/conflagrant/InputManager.hh
        src/conflagrant/glfw/GlfwWindow.hh
        src/conflagrant/glfw/imgui_impl_glfw_gl3.h
        src/conflagrant/DoubleBuffer.hh
        )
set(CC_FILES
        src/conflagrant/args.cc
        src/conflagrant/Engine.cc
        src/conflagrant/Time.cc
        src/conflagrant/logging.cc
        src/conflagrant/geometry.cc
        src/conflagrant/ShaderSourceManager.cc
        src/conflagrant/assets/AssetManager.cc
        src/conflagrant/assets/loaders/ModelLoader.cc
        src/conflagrant/assets/loaders/TextureLoader.cc
        src/conflagrant/conflagrant.cc
        src/conflagrant/CL.cc
        src/conflagrant/GL.cc
        src/conflagrant/glfw/GlfwWindow.cc
        src/conflagrant/InputManager.cc
        src/conflagrant/Factory.cc
        src/conflagrant/systems/Animator.cc
        src/conflagrant/systems/CameraController.cc
        src/conflagrant/systems/EcsDebugger.cc
        src/conflagrant/systems/DeferredRenderer.cc
        src/conflagrant/systems/ForwardRenderer.cc
        src/conflagrant/systems/SnowfallAnimator.cc
        src/conflagrant/glfw/imgui_impl_glfw_gl3.cpp
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
