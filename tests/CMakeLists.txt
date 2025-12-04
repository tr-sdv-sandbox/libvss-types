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

# VSS integration test (requires nlohmann/json)
# This test validates that libvss-types supports the complete VSS specification
# NOTE: JSON parsing is ONLY used in tests - the library itself has no JSON dependency

# Try to find nlohmann_json system-wide first (any version >= 3.2.0)
find_package(nlohmann_json 3.2.0 QUIET)

if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found system-wide, skipping VSS integration test")
    message(STATUS "  Install nlohmann-json3-dev to enable this test")
    set(VSS_INTEGRATION_TEST_ENABLED FALSE)
else()
    set(VSS_INTEGRATION_TEST_ENABLED TRUE)
endif()

# Build VSS integration test (only if nlohmann_json is available)
if(VSS_INTEGRATION_TEST_ENABLED)
    add_executable(test_vss_integration test_vss_integration.cpp)
    target_link_libraries(test_vss_integration
        PRIVATE
            vss::types
            nlohmann_json::nlohmann_json
            GTest::gtest
            GTest::gtest_main
    )

    # Copy vss_test.json to build directory for test execution
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/vss_test.json
        ${CMAKE_CURRENT_BINARY_DIR}/vss_test.json
        COPYONLY
    )

    gtest_discover_tests(test_vss_integration)
    message(STATUS "VSS integration test enabled")
endif()

include(GoogleTest)
gtest_discover_tests(test_value)
gtest_discover_tests(test_struct)
gtest_discover_tests(test_quality)
gtest_discover_tests(test_struct_advanced)
