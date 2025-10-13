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

# Try to find nlohmann_json system-wide first
find_package(nlohmann_json 3.11.0 QUIET)

if(NOT nlohmann_json_FOUND)
    # Fall back to FetchContent to download it
    message(STATUS "nlohmann_json not found system-wide, fetching from GitHub...")
    include(FetchContent)

    # Use FetchContent to download the single header file
    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
        URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
    )

    FetchContent_MakeAvailable(nlohmann_json)

    # nlohmann_json provides nlohmann_json::nlohmann_json target automatically
    message(STATUS "nlohmann_json downloaded to build directory")
endif()

# Build VSS integration test
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

include(GoogleTest)
gtest_discover_tests(test_value)
gtest_discover_tests(test_struct)
gtest_discover_tests(test_quality)
gtest_discover_tests(test_struct_advanced)
