cmake_minimum_required(VERSION 3.16)

project(libvss-types
    VERSION 0.1.0
    DESCRIPTION "Backend-agnostic VSS type definitions for C++"
    LANGUAGES CXX
)

# C++17 required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(VSS_TYPES_BUILD_TESTS "Build tests" ON)
option(VSS_TYPES_BUILD_EXAMPLES "Build examples" ON)

# Library target
add_library(vss-types
    src/value.cpp
    src/struct.cpp
    src/quality.cpp
)

# Alias for consistent naming
add_library(vss::types ALIAS vss-types)

# Include directories
target_include_directories(vss-types
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Compiler warnings
target_compile_options(vss-types PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Installation
include(GNUInstallDirs)

# Set the export name to create vss::types target (must be before install)
set_target_properties(vss-types PROPERTIES EXPORT_NAME types)

install(TARGETS vss-types
    EXPORT vss-types-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/vss
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export configuration
# Export with custom target name "types" so namespace creates "vss::types"
install(EXPORT vss-types-targets
    FILE vss-types-targets.cmake
    NAMESPACE vss::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vss-types
)

# Package config
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/vss-types-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/vss-types-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vss-types
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/vss-types-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vss-types-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/vss-types-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vss-types
)

# Tests
if(VSS_TYPES_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(VSS_TYPES_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/vss-types.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/vss-types.pc
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vss-types.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
