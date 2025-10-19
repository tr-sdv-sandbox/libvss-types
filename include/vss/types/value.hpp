/**
 * @file value.hpp
 * @brief Core VSS value type definitions
 *
 * This file defines the fundamental value types used across VSS implementations.
 * It is backend-agnostic and contains no protobuf or KUKSA-specific code.
 */

#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <memory>
#include <optional>

namespace vss::types {

// Forward declarations
class StructValue;

/**
 * @brief VSS value type - supports primitives, arrays, and structs
 *
 * This variant represents all possible VSS value types:
 * - Primitive types (bool, integers, floats, string)
 * - Array types (vectors of primitives)
 * - Struct types (nested structured data)
 *
 * Note: std::monostate represents "no value" (different from invalid)
 */
using Value = std::variant<
    std::monostate,              // No value (empty/uninitialized)
    // Primitive types
    bool,
    int8_t,
    int16_t,
    int32_t,
    int64_t,
    uint8_t,
    uint16_t,
    uint32_t,
    uint64_t,
    float,
    double,
    std::string,
    // Array types
    std::vector<bool>,
    std::vector<int8_t>,
    std::vector<int16_t>,
    std::vector<int32_t>,
    std::vector<int64_t>,
    std::vector<uint8_t>,
    std::vector<uint16_t>,
    std::vector<uint32_t>,
    std::vector<uint64_t>,
    std::vector<float>,
    std::vector<double>,
    std::vector<std::string>,
    // Struct types
    std::shared_ptr<StructValue>, // Struct (heap-allocated to avoid circular dependency)
    std::vector<std::shared_ptr<StructValue>> // Array of structs
>;

/**
 * @brief Enum representing all VSS data types
 *
 * Values are aligned with KUKSA DataType protobuf enum for compatibility,
 * but this library doesn't depend on protobuf.
 *
 * Range allocation:
 * - 0: Unspecified/unknown
 * - 1-19: Primitive types
 * - 20-39: Array types
 * - 40-59: Struct types (future expansion)
 */
enum class ValueType {
    UNSPECIFIED  = 0,   // Unknown or not set

    // Primitive types (matching KUKSA protobuf values where applicable)
    STRING       = 1,
    BOOL         = 2,
    INT8         = 3,
    INT16        = 4,
    INT32        = 5,
    INT64        = 6,
    UINT8        = 7,
    UINT16       = 8,
    UINT32       = 9,
    UINT64       = 10,
    FLOAT        = 11,
    DOUBLE       = 12,

    // Array types
    STRING_ARRAY = 20,
    BOOL_ARRAY   = 21,
    INT8_ARRAY   = 22,
    INT16_ARRAY  = 23,
    INT32_ARRAY  = 24,
    INT64_ARRAY  = 25,
    UINT8_ARRAY  = 26,
    UINT16_ARRAY = 27,
    UINT32_ARRAY = 28,
    UINT64_ARRAY = 29,
    FLOAT_ARRAY  = 30,
    DOUBLE_ARRAY = 31,

    // Struct types (VSS 4.0)
    STRUCT       = 40,  // Single struct instance
    STRUCT_ARRAY = 41   // Array of struct instances
};

/**
 * @brief Get ValueType enum from C++ type at compile time
 *
 * Example:
 * @code
 * constexpr auto t = get_value_type<float>();  // ValueType::FLOAT
 * @endcode
 */
template<typename T>
constexpr ValueType get_value_type() {
    if constexpr (std::is_same_v<T, bool>) return ValueType::BOOL;
    else if constexpr (std::is_same_v<T, int8_t>) return ValueType::INT8;
    else if constexpr (std::is_same_v<T, int16_t>) return ValueType::INT16;
    else if constexpr (std::is_same_v<T, int32_t>) return ValueType::INT32;
    else if constexpr (std::is_same_v<T, int64_t>) return ValueType::INT64;
    else if constexpr (std::is_same_v<T, uint8_t>) return ValueType::UINT8;
    else if constexpr (std::is_same_v<T, uint16_t>) return ValueType::UINT16;
    else if constexpr (std::is_same_v<T, uint32_t>) return ValueType::UINT32;
    else if constexpr (std::is_same_v<T, uint64_t>) return ValueType::UINT64;
    else if constexpr (std::is_same_v<T, float>) return ValueType::FLOAT;
    else if constexpr (std::is_same_v<T, double>) return ValueType::DOUBLE;
    else if constexpr (std::is_same_v<T, std::string>) return ValueType::STRING;
    else if constexpr (std::is_same_v<T, std::vector<bool>>) return ValueType::BOOL_ARRAY;
    else if constexpr (std::is_same_v<T, std::vector<int8_t>>) return ValueType::INT8_ARRAY;
    else if constexpr (std::is_same_v<T, std::vector<int16_t>>) return ValueType::INT16_ARRAY;
    else if constexpr (std::is_same_v<T, std::vector<int32_t>>) return ValueType::INT32_ARRAY;
    else if constexpr (std::is_same_v<T, std::vector<int64_t>>) return ValueType::INT64_ARRAY;
    else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) return ValueType::UINT8_ARRAY;
    else if constexpr (std::is_same_v<T, std::vector<uint16_t>>) return ValueType::UINT16_ARRAY;
    else if constexpr (std::is_same_v<T, std::vector<uint32_t>>) return ValueType::UINT32_ARRAY;
    else if constexpr (std::is_same_v<T, std::vector<uint64_t>>) return ValueType::UINT64_ARRAY;
    else if constexpr (std::is_same_v<T, std::vector<float>>) return ValueType::FLOAT_ARRAY;
    else if constexpr (std::is_same_v<T, std::vector<double>>) return ValueType::DOUBLE_ARRAY;
    else if constexpr (std::is_same_v<T, std::vector<std::string>>) return ValueType::STRING_ARRAY;
    else if constexpr (std::is_same_v<T, StructValue>) return ValueType::STRUCT;
    else if constexpr (std::is_same_v<T, std::shared_ptr<StructValue>>) return ValueType::STRUCT;
    else if constexpr (std::is_same_v<T, std::vector<std::shared_ptr<StructValue>>>) return ValueType::STRUCT_ARRAY;
    else {
        static_assert(sizeof(T) == 0, "Unsupported type for VSS Value");
    }
}

/**
 * @brief Get ValueType from a Value variant at runtime
 *
 * @param value The value to inspect
 * @return ValueType corresponding to the active variant alternative
 */
ValueType get_value_type(const Value& value);

/**
 * @brief Convert ValueType enum to string representation
 *
 * Useful for logging, YAML output, debugging.
 *
 * @param type The type to convert
 * @return String representation (e.g., "FLOAT", "STRING_ARRAY", "STRUCT")
 */
const char* value_type_to_string(ValueType type);

/**
 * @brief Parse ValueType from string
 *
 * Case-insensitive matching for common variants.
 * Useful for YAML/JSON parsing.
 *
 * @param str String representation (e.g., "float", "BOOL", "struct")
 * @return ValueType if recognized, std::nullopt otherwise
 */
std::optional<ValueType> value_type_from_string(const std::string& str);

/**
 * @brief Check if two types are compatible for VSS operations
 *
 * Types are compatible if they can safely interchange in VSS context:
 * - Floating point: float ↔ double
 * - Signed integers: int32_t ↔ int64_t
 * - Unsigned integers: uint32_t ↔ uint64_t
 * - Arrays: compatible if element types are compatible
 *
 * Incompatible combinations:
 * - Different type families (int ↔ float, bool ↔ int, string ↔ numeric)
 * - Scalar ↔ array (unless one is UNSPECIFIED)
 * - Signed ↔ unsigned integers
 * - Structs with different type names
 *
 * @param expected The expected type
 * @param actual The actual type
 * @return true if types are compatible, false otherwise
 */
bool are_types_compatible(ValueType expected, ValueType actual);

/**
 * @brief Check if value is empty (std::monostate)
 *
 * @param value The value to check
 * @return true if value holds std::monostate, false otherwise
 */
inline bool is_empty(const Value& value) {
    return std::holds_alternative<std::monostate>(value);
}

/**
 * @brief Check if type is a primitive (non-array, non-struct)
 *
 * @param type The type to check
 * @return true if type is a primitive scalar
 */
inline bool is_primitive(ValueType type) {
    return type >= ValueType::STRING && type <= ValueType::DOUBLE;
}

/**
 * @brief Check if type is an array type
 *
 * @param type The type to check
 * @return true if type is an array (including struct arrays)
 */
inline bool is_array(ValueType type) {
    return (type >= ValueType::STRING_ARRAY && type <= ValueType::DOUBLE_ARRAY) ||
           type == ValueType::STRUCT_ARRAY;
}

/**
 * @brief Check if type is a struct or struct array
 *
 * @param type The type to check
 * @return true if type is STRUCT or STRUCT_ARRAY
 */
inline bool is_struct(ValueType type) {
    return type == ValueType::STRUCT || type == ValueType::STRUCT_ARRAY;
}

} // namespace vss::types
