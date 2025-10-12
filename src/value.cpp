/**
 * @file value.cpp
 * @brief Implementation of value type utilities
 */

#include <vss/types/value.hpp>
#include <vss/types/struct.hpp>
#include <algorithm>
#include <cctype>

namespace vss::types {

ValueType get_value_type(const Value& value) {
    return std::visit([](auto&& arg) -> ValueType {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            return ValueType::UNSPECIFIED;
        } else if constexpr (std::is_same_v<T, bool>) {
            return ValueType::BOOL;
        } else if constexpr (std::is_same_v<T, int32_t>) {
            return ValueType::INT32;
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            return ValueType::UINT32;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return ValueType::INT64;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return ValueType::UINT64;
        } else if constexpr (std::is_same_v<T, float>) {
            return ValueType::FLOAT;
        } else if constexpr (std::is_same_v<T, double>) {
            return ValueType::DOUBLE;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return ValueType::STRING;
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            return ValueType::BOOL_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<int32_t>>) {
            return ValueType::INT32_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<uint32_t>>) {
            return ValueType::UINT32_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            return ValueType::INT64_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<uint64_t>>) {
            return ValueType::UINT64_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<float>>) {
            return ValueType::FLOAT_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return ValueType::DOUBLE_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return ValueType::STRING_ARRAY;
        } else if constexpr (std::is_same_v<T, std::shared_ptr<StructValue>>) {
            return ValueType::STRUCT;
        } else if constexpr (std::is_same_v<T, std::vector<std::shared_ptr<StructValue>>>) {
            return ValueType::STRUCT_ARRAY;
        } else {
            return ValueType::UNSPECIFIED;
        }
    }, value);
}

const char* value_type_to_string(ValueType type) {
    switch (type) {
        case ValueType::UNSPECIFIED:  return "UNSPECIFIED";
        case ValueType::STRING:       return "STRING";
        case ValueType::BOOL:         return "BOOL";
        case ValueType::INT32:        return "INT32";
        case ValueType::INT64:        return "INT64";
        case ValueType::UINT32:       return "UINT32";
        case ValueType::UINT64:       return "UINT64";
        case ValueType::FLOAT:        return "FLOAT";
        case ValueType::DOUBLE:       return "DOUBLE";
        case ValueType::STRING_ARRAY: return "STRING_ARRAY";
        case ValueType::BOOL_ARRAY:   return "BOOL_ARRAY";
        case ValueType::INT32_ARRAY:  return "INT32_ARRAY";
        case ValueType::INT64_ARRAY:  return "INT64_ARRAY";
        case ValueType::UINT32_ARRAY: return "UINT32_ARRAY";
        case ValueType::UINT64_ARRAY: return "UINT64_ARRAY";
        case ValueType::FLOAT_ARRAY:  return "FLOAT_ARRAY";
        case ValueType::DOUBLE_ARRAY: return "DOUBLE_ARRAY";
        case ValueType::STRUCT:       return "STRUCT";
        case ValueType::STRUCT_ARRAY: return "STRUCT_ARRAY";
        default:                      return "UNKNOWN";
    }
}

std::optional<ValueType> value_type_from_string(const std::string& str) {
    // Convert to uppercase for case-insensitive matching
    std::string upper = str;
    std::transform(upper.begin(), upper.end(), upper.begin(),
                   [](unsigned char c){ return std::toupper(c); });

    if (upper == "STRING") return ValueType::STRING;
    if (upper == "BOOL" || upper == "BOOLEAN") return ValueType::BOOL;
    if (upper == "INT32" || upper == "INT") return ValueType::INT32;
    if (upper == "INT64" || upper == "LONG") return ValueType::INT64;
    if (upper == "UINT32" || upper == "UNSIGNED") return ValueType::UINT32;
    if (upper == "UINT64" || upper == "ULONG") return ValueType::UINT64;
    if (upper == "FLOAT") return ValueType::FLOAT;
    if (upper == "DOUBLE") return ValueType::DOUBLE;
    if (upper == "STRING_ARRAY" || upper == "STRING[]") return ValueType::STRING_ARRAY;
    if (upper == "BOOL_ARRAY" || upper == "BOOL[]" || upper == "BOOLEAN[]") return ValueType::BOOL_ARRAY;
    if (upper == "INT32_ARRAY" || upper == "INT32[]" || upper == "INT[]") return ValueType::INT32_ARRAY;
    if (upper == "INT64_ARRAY" || upper == "INT64[]" || upper == "LONG[]") return ValueType::INT64_ARRAY;
    if (upper == "UINT32_ARRAY" || upper == "UINT32[]") return ValueType::UINT32_ARRAY;
    if (upper == "UINT64_ARRAY" || upper == "UINT64[]") return ValueType::UINT64_ARRAY;
    if (upper == "FLOAT_ARRAY" || upper == "FLOAT[]") return ValueType::FLOAT_ARRAY;
    if (upper == "DOUBLE_ARRAY" || upper == "DOUBLE[]") return ValueType::DOUBLE_ARRAY;
    if (upper == "STRUCT") return ValueType::STRUCT;
    if (upper == "STRUCT_ARRAY" || upper == "STRUCT[]") return ValueType::STRUCT_ARRAY;

    return std::nullopt;
}

bool are_types_compatible(ValueType expected, ValueType actual) {
    if (expected == actual) return true;
    if (expected == ValueType::UNSPECIFIED || actual == ValueType::UNSPECIFIED) return true;

    // Floating point compatibility: float ↔ double
    if ((expected == ValueType::FLOAT && actual == ValueType::DOUBLE) ||
        (expected == ValueType::DOUBLE && actual == ValueType::FLOAT)) {
        return true;
    }

    // Signed integer compatibility: int32 ↔ int64
    if ((expected == ValueType::INT32 && actual == ValueType::INT64) ||
        (expected == ValueType::INT64 && actual == ValueType::INT32)) {
        return true;
    }

    // Unsigned integer compatibility: uint32 ↔ uint64
    if ((expected == ValueType::UINT32 && actual == ValueType::UINT64) ||
        (expected == ValueType::UINT64 && actual == ValueType::UINT32)) {
        return true;
    }

    // Floating point array compatibility: float[] ↔ double[]
    if ((expected == ValueType::FLOAT_ARRAY && actual == ValueType::DOUBLE_ARRAY) ||
        (expected == ValueType::DOUBLE_ARRAY && actual == ValueType::FLOAT_ARRAY)) {
        return true;
    }

    // Signed integer array compatibility: int32[] ↔ int64[]
    if ((expected == ValueType::INT32_ARRAY && actual == ValueType::INT64_ARRAY) ||
        (expected == ValueType::INT64_ARRAY && actual == ValueType::INT32_ARRAY)) {
        return true;
    }

    // Unsigned integer array compatibility: uint32[] ↔ uint64[]
    if ((expected == ValueType::UINT32_ARRAY && actual == ValueType::UINT64_ARRAY) ||
        (expected == ValueType::UINT64_ARRAY && actual == ValueType::UINT32_ARRAY)) {
        return true;
    }

    return false;
}

} // namespace vss::types
