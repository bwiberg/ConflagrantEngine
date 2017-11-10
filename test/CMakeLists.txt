#### GoogleTest
add_subdirectory(lib/gtest-1.8.0)

## Macro to create folder-specific test executable
macro(create_test filename_without_extension)
    set(exe_name ${filename_without_extension})
    set(file_name "${filename_without_extension}.cc")

    include_directories(${gtest_SOURCE_DIR}/include
            ${gtest_SOURCE_DIR}
            ${CFL_INCLUDE_DIRS}
            helpers)

    set(ALL_TESTS_CC ${ALL_TESTS_CC} ${file_name})

    add_executable(${exe_name} ${file_name})
    target_link_libraries(${exe_name}
            gtest
            gtest_main
            ${CFL_LIBRARIES}
            )
endmacro()

#### Create test suites
create_test(test_Serialization)

#### Create executable with all tests
include_directories(
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ${CFL_INCLUDE_DIRS}
)

if (${ALL_TESTS_CC})
    add_executable(all_tests ${ALL_TESTS_CC})
    target_link_libraries(all_tests
            gtest
            gtest_main
            ${CFL_LIBRARIES}
            )
endif ()
