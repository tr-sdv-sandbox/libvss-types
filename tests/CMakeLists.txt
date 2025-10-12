find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "GTest not found, tests will not be built")
    return()
endif()

add_executable(test_value test_value.cpp)
target_link_libraries(test_value
    PRIVATE
        vss::types
        GTest::gtest
        GTest::gtest_main
)

add_executable(test_struct test_struct.cpp)
target_link_libraries(test_struct
    PRIVATE
        vss::types
        GTest::gtest
        GTest::gtest_main
)

add_executable(test_quality test_quality.cpp)
target_link_libraries(test_quality
    PRIVATE
        vss::types
        GTest::gtest
        GTest::gtest_main
)

add_executable(test_struct_advanced test_struct_advanced.cpp)
target_link_libraries(test_struct_advanced
    PRIVATE
        vss::types
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_value)
gtest_discover_tests(test_struct)
gtest_discover_tests(test_quality)
gtest_discover_tests(test_struct_advanced)
