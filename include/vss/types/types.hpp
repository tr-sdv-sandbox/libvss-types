/**
 * @file types.hpp
 * @brief Main header for libvss-types - include this to use the library
 *
 * libvss-types provides backend-agnostic VSS type definitions for C++.
 * It supports VSS 4.0 including structs, arrays, and signal quality.
 *
 * This library is designed to be shared between:
 * - libVSSDAG (CAN → VSS transformation)
 * - kuksa-cpp-client (KUKSA databroker client)
 * - Other VSS implementations
 *
 * Key features:
 * - Type-safe Value variant (primitives, arrays, structs)
 * - Struct definitions and validation (VSS 4.0)
 * - Signal quality indicators (VALID/INVALID/NOT_AVAILABLE)
 * - Type utilities and conversions
 * - NO dependencies on protobuf, gRPC, or KUKSA
 *
 * Example:
 * @code
 * using namespace vss::types;
 *
 * // Create a simple value
 * Value speed = 120.5f;
 * auto type = get_value_type(speed);  // ValueType::FLOAT
 *
 * // Create a struct
 * StructValue delivery{"DeliveryInfo"};
 * delivery.set_field("Address", std::string("123 Main St"));
 * delivery.set_field("Receiver", std::string("John Doe"));
 *
 * // Use qualified values
 * QualifiedValue<float> temp{22.5f, SignalQuality::VALID};
 * if (temp.is_valid()) {
 *     std::cout << "Temperature: " << *temp.value << "\n";
 * }
 * @endcode
 */

#pragma once

// Core types
#include "value.hpp"
#include "struct.hpp"
#include "quality.hpp"

/**
 * @namespace vss::types
 * @brief VSS type system
 *
 * Backend-agnostic types for representing VSS signals and structures.
 */
namespace vss::types {

/**
 * @brief Library version information
 */
struct Version {
    static constexpr int MAJOR = 0;
    static constexpr int MINOR = 1;
    static constexpr int PATCH = 0;

    static const char* string() {
        return "0.1.0";
    }
};

} // namespace vss::types
