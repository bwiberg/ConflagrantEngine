if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(assimp_ARCHITECTURE "64")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(assimp_ARCHITECTURE "32")
endif (CMAKE_SIZEOF_VOID_P EQUAL 8)

if (WIN32)
    set(assimp_ROOT_DIR CACHE PATH "ASSIMP root directory")

    # Find path of each library
    find_path(assimp_INCLUDE_DIRS
            NAMES
            assimp/anim.h
            HINTS
            ${assimp_ROOT_DIR}/include
            $ENV{PATH}/include
            )

    if (MSVC12)
        set(assimp_MSVC_VERSION "vc120")
    elseif (MSVC14)
        set(assimp_MSVC_VERSION "vc140")
    endif (MSVC12)

    if (MSVC12 OR MSVC14)
        find_path(assimp_LIBRARY_DIR
                NAMES
                assimp-${assimp_MSVC_VERSION}-mt.lib
                assimp
                HINTS
                ${assimp_ROOT_DIR}/lib${assimp_ARCHITECTURE}
                )

        find_library(assimp_LIBRARY_RELEASE assimp-${assimp_MSVC_VERSION}-mt.lib PATHS ${assimp_LIBRARY_DIR})
        find_library(assimp_LIBRARY_DEBUG assimp-${assimp_MSVC_VERSION}-mtd.lib PATHS ${assimp_LIBRARY_DIR})

        set(assimp_LIBRARY
                optimized ${assimp_LIBRARY_RELEASE}
                debug ${assimp_LIBRARY_DEBUG}
                )

        set(assimp_LIBRARIES "assimp_LIBRARY_RELEASE" "assimp_LIBRARY_DEBUG")

        FUNCTION(assimp_COPY_BINARIES TargetDirectory)
            ADD_CUSTOM_TARGET(AssimpCopyBinaries
                    COMMAND ${CMAKE_COMMAND} -E copy ${assimp_ROOT_DIR}/bin${assimp_ARCHITECTURE}/assimp-${assimp_MSVC_VERSION}-mtd.dll ${TargetDirectory}/Debug/assimp-${assimp_MSVC_VERSION}-mtd.dll
                    COMMAND ${CMAKE_COMMAND} -E copy ${assimp_ROOT_DIR}/bin${assimp_ARCHITECTURE}/assimp-${assimp_MSVC_VERSION}-mt.dll ${TargetDirectory}/Release/assimp-${assimp_MSVC_VERSION}-mt.dll
                    COMMENT "Copying Assimp binaries to '${TargetDirectory}'"
                    VERBATIM)
        ENDFUNCTION(assimp_COPY_BINARIES)
    else ()
        find_library(assimp_LIBRARY_ANY
                NAMES
                assimp
                PATHS
                $ENV{PATH}/lib)
        set(assimp_LIBRARIES ${assimp_LIBRARY_ANY})
    endif ()

else (WIN32)

    find_path(
            assimp_INCLUDE_DIRS
            NAMES
            assimp/postprocess.h
            assimp/scene.h
            assimp/version.h
            assimp/config.h
            assimp/cimport.h
            PATHS /usr/local/include/ /usr/include/
    )

    find_library(
            assimp_LIBRARIES
            NAMES assimp
            PATHS /usr/local/lib/ /usr/lib/
    )
endif (WIN32)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(assimp
        REQUIRED_VARS assimp_INCLUDE_DIRS assimp_LIBRARIES)
