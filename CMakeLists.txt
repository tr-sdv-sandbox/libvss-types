cmake_minimum_required(VERSION 3.16)

project(libvss-types
    VERSION 0.1.0
    DESCRIPTION "Backend-agnostic VSS type definitions for C++"
    LANGUAGES CXX C
)

# C++17 required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(VSS_TYPES_BUILD_TESTS "Build tests" ON)
option(VSS_TYPES_BUILD_EXAMPLES "Build examples" ON)
option(VSS_TYPES_BUILD_DDS "Build DDS types library (requires CycloneDDS)" ON)

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

# =============================================================================
# Optional DDS Support
# =============================================================================
# When CycloneDDS is available and VSS_TYPES_BUILD_DDS is ON, we compile
# the IDL files and create a separate vss-types-dds library.
#
# IDL files are always installed regardless of DDS availability, so users
# can compile them with their own DDS implementation.
# =============================================================================

# Always install IDL files (no DDS dependency needed)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/idl/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/vss-types/idl
    FILES_MATCHING PATTERN "*.idl"
)

if(VSS_TYPES_BUILD_DDS)
    find_package(CycloneDDS QUIET)

    if(CycloneDDS_FOUND)
        message(STATUS "CycloneDDS found - building vss-types-dds library")

        # Find idlc compiler
        find_program(IDLC_EXECUTABLE idlc REQUIRED)

        # Output directory for generated files
        set(VSS_DDS_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/dds)
        file(MAKE_DIRECTORY ${VSS_DDS_GEN_DIR})

        # IDL files to compile
        set(VSS_IDL_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/idl/vss_types.idl
            ${CMAKE_CURRENT_SOURCE_DIR}/idl/vss_signal.idl
        )

        # Generated source files (idlc generates .c and .h for each IDL)
        set(VSS_DDS_GENERATED_SOURCES
            ${VSS_DDS_GEN_DIR}/vss_types.c
            ${VSS_DDS_GEN_DIR}/vss_signal.c
        )
        set(VSS_DDS_GENERATED_HEADERS
            ${VSS_DDS_GEN_DIR}/vss_types.h
            ${VSS_DDS_GEN_DIR}/vss_signal.h
        )

        # Custom command to compile IDL files
        add_custom_command(
            OUTPUT ${VSS_DDS_GENERATED_SOURCES} ${VSS_DDS_GENERATED_HEADERS}
            COMMAND ${IDLC_EXECUTABLE} -l c
                    -I ${CMAKE_CURRENT_SOURCE_DIR}/idl
                    ${CMAKE_CURRENT_SOURCE_DIR}/idl/vss_types.idl
            COMMAND ${IDLC_EXECUTABLE} -l c
                    -I ${CMAKE_CURRENT_SOURCE_DIR}/idl
                    ${CMAKE_CURRENT_SOURCE_DIR}/idl/vss_signal.idl
            WORKING_DIRECTORY ${VSS_DDS_GEN_DIR}
            DEPENDS ${VSS_IDL_FILES}
            COMMENT "Generating DDS types from VSS IDL"
            VERBATIM
        )

        # Custom target for IDL generation
        add_custom_target(vss-types-dds-generate
            DEPENDS ${VSS_DDS_GENERATED_SOURCES} ${VSS_DDS_GENERATED_HEADERS}
        )

        # DDS types library
        # Note: We need to set LINKER_LANGUAGE since sources are generated
        add_library(vss-types-dds STATIC
            ${VSS_DDS_GENERATED_SOURCES}
        )
        set_target_properties(vss-types-dds PROPERTIES LINKER_LANGUAGE C)

        add_library(vss::types-dds ALIAS vss-types-dds)

        add_dependencies(vss-types-dds vss-types-dds-generate)

        target_include_directories(vss-types-dds
            PUBLIC
                $<BUILD_INTERFACE:${VSS_DDS_GEN_DIR}>
                $<INSTALL_INTERFACE:include/vss/dds>
        )

        target_link_libraries(vss-types-dds
            PUBLIC
                CycloneDDS::ddsc
        )

        # Install DDS library
        install(TARGETS vss-types-dds
            EXPORT vss-types-dds-targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

        # Install generated headers
        install(FILES ${VSS_DDS_GENERATED_HEADERS}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vss/dds
        )

        # Export DDS targets
        install(EXPORT vss-types-dds-targets
            FILE vss-types-dds-targets.cmake
            NAMESPACE vss::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vss-types
        )

        set(VSS_TYPES_DDS_AVAILABLE TRUE CACHE INTERNAL "VSS DDS types available")
    else()
        message(STATUS "CycloneDDS not found - skipping vss-types-dds library")
        message(STATUS "  IDL files will still be installed for manual compilation")
        set(VSS_TYPES_DDS_AVAILABLE FALSE CACHE INTERNAL "VSS DDS types available")
    endif()
endif()
