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
        } else if constexpr (std::is_same_v<T, int8_t>) {
            return ValueType::INT8;
        } else if constexpr (std::is_same_v<T, int16_t>) {
            return ValueType::INT16;
        } else if constexpr (std::is_same_v<T, int32_t>) {
            return ValueType::INT32;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return ValueType::INT64;
        } else if constexpr (std::is_same_v<T, uint8_t>) {
            return ValueType::UINT8;
        } else if constexpr (std::is_same_v<T, uint16_t>) {
            return ValueType::UINT16;
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            return ValueType::UINT32;
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
        } else if constexpr (std::is_same_v<T, std::vector<int8_t>>) {
            return ValueType::INT8_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<int16_t>>) {
            return ValueType::INT16_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<int32_t>>) {
            return ValueType::INT32_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            return ValueType::INT64_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
            return ValueType::UINT8_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<uint16_t>>) {
            return ValueType::UINT16_ARRAY;
        } else if constexpr (std::is_same_v<T, std::vector<uint32_t>>) {
            return ValueType::UINT32_ARRAY;
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
        case ValueType::INT8:         return "INT8";
        case ValueType::INT16:        return "INT16";
        case ValueType::INT32:        return "INT32";
        case ValueType::INT64:        return "INT64";
        case ValueType::UINT8:        return "UINT8";
        case ValueType::UINT16:       return "UINT16";
        case ValueType::UINT32:       return "UINT32";
        case ValueType::UINT64:       return "UINT64";
        case ValueType::FLOAT:        return "FLOAT";
        case ValueType::DOUBLE:       return "DOUBLE";
        case ValueType::STRING_ARRAY: return "STRING_ARRAY";
        case ValueType::BOOL_ARRAY:   return "BOOL_ARRAY";
        case ValueType::INT8_ARRAY:   return "INT8_ARRAY";
        case ValueType::INT16_ARRAY:  return "INT16_ARRAY";
        case ValueType::INT32_ARRAY:  return "INT32_ARRAY";
        case ValueType::INT64_ARRAY:  return "INT64_ARRAY";
        case ValueType::UINT8_ARRAY:  return "UINT8_ARRAY";
        case ValueType::UINT16_ARRAY: return "UINT16_ARRAY";
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
    std::string upper;
    upper.reserve(str.size());
    upper = str;
    std::transform(upper.begin(), upper.end(), upper.begin(),
                   [](unsigned char c){ return std::toupper(c); });

    if (upper == "STRING") return ValueType::STRING;
    if (upper == "BOOL" || upper == "BOOLEAN") return ValueType::BOOL;
    if (upper == "INT8") return ValueType::INT8;
    if (upper == "INT16") return ValueType::INT16;
    if (upper == "INT32" || upper == "INT") return ValueType::INT32;
    if (upper == "INT64" || upper == "LONG") return ValueType::INT64;
    if (upper == "UINT8") return ValueType::UINT8;
    if (upper == "UINT16") return ValueType::UINT16;
    if (upper == "UINT32" || upper == "UNSIGNED") return ValueType::UINT32;
    if (upper == "UINT64" || upper == "ULONG") return ValueType::UINT64;
    if (upper == "FLOAT") return ValueType::FLOAT;
    if (upper == "DOUBLE") return ValueType::DOUBLE;
    if (upper == "STRING_ARRAY" || upper == "STRING[]") return ValueType::STRING_ARRAY;
    if (upper == "BOOL_ARRAY" || upper == "BOOL[]" || upper == "BOOLEAN[]") return ValueType::BOOL_ARRAY;
    if (upper == "INT8_ARRAY" || upper == "INT8[]") return ValueType::INT8_ARRAY;
    if (upper == "INT16_ARRAY" || upper == "INT16[]") return ValueType::INT16_ARRAY;
    if (upper == "INT32_ARRAY" || upper == "INT32[]" || upper == "INT[]") return ValueType::INT32_ARRAY;
    if (upper == "INT64_ARRAY" || upper == "INT64[]" || upper == "LONG[]") return ValueType::INT64_ARRAY;
    if (upper == "UINT8_ARRAY" || upper == "UINT8[]") return ValueType::UINT8_ARRAY;
    if (upper == "UINT16_ARRAY" || upper == "UINT16[]") return ValueType::UINT16_ARRAY;
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

    // Signed integer compatibility: int8 ↔ int16 ↔ int32 ↔ int64
    if ((expected == ValueType::INT8 || expected == ValueType::INT16 ||
         expected == ValueType::INT32 || expected == ValueType::INT64) &&
        (actual == ValueType::INT8 || actual == ValueType::INT16 ||
         actual == ValueType::INT32 || actual == ValueType::INT64)) {
        return true;
    }

    // Unsigned integer compatibility: uint8 ↔ uint16 ↔ uint32 ↔ uint64
    if ((expected == ValueType::UINT8 || expected == ValueType::UINT16 ||
         expected == ValueType::UINT32 || expected == ValueType::UINT64) &&
        (actual == ValueType::UINT8 || actual == ValueType::UINT16 ||
         actual == ValueType::UINT32 || actual == ValueType::UINT64)) {
        return true;
    }

    // Floating point array compatibility: float[] ↔ double[]
    if ((expected == ValueType::FLOAT_ARRAY && actual == ValueType::DOUBLE_ARRAY) ||
        (expected == ValueType::DOUBLE_ARRAY && actual == ValueType::FLOAT_ARRAY)) {
        return true;
    }

    // Signed integer array compatibility: int8[] ↔ int16[] ↔ int32[] ↔ int64[]
    if ((expected == ValueType::INT8_ARRAY || expected == ValueType::INT16_ARRAY ||
         expected == ValueType::INT32_ARRAY || expected == ValueType::INT64_ARRAY) &&
        (actual == ValueType::INT8_ARRAY || actual == ValueType::INT16_ARRAY ||
         actual == ValueType::INT32_ARRAY || actual == ValueType::INT64_ARRAY)) {
        return true;
    }

    // Unsigned integer array compatibility: uint8[] ↔ uint16[] ↔ uint32[] ↔ uint64[]
    if ((expected == ValueType::UINT8_ARRAY || expected == ValueType::UINT16_ARRAY ||
         expected == ValueType::UINT32_ARRAY || expected == ValueType::UINT64_ARRAY) &&
        (actual == ValueType::UINT8_ARRAY || actual == ValueType::UINT16_ARRAY ||
         actual == ValueType::UINT32_ARRAY || actual == ValueType::UINT64_ARRAY)) {
        return true;
    }

    return false;
}

Value convert_value_type(const Value& value, ValueType target_type) {
    // Get current type
    ValueType current_type = get_value_type(value);

    // If already the right type, return unchanged
    if (current_type == target_type) {
        return value;
    }

    // If empty, return empty
    if (current_type == ValueType::UNSPECIFIED) {
        return value;
    }

    // Check if types are compatible
    if (!are_types_compatible(target_type, current_type)) {
        return Value{std::monostate{}};
    }

    // Perform conversion based on target type
    return std::visit([target_type](auto&& val) -> Value {
        using T = std::decay_t<decltype(val)>;

        // Handle monostate
        if constexpr (std::is_same_v<T, std::monostate>) {
            return Value{std::monostate{}};
        }

        // Signed integer conversions
        else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> ||
                           std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>) {
            int64_t wide_val = static_cast<int64_t>(val);

            switch (target_type) {
                case ValueType::INT8:
                    if (wide_val < INT8_MIN || wide_val > INT8_MAX) {
                        return Value{std::monostate{}};  // Out of range
                    }
                    return Value{static_cast<int8_t>(wide_val)};

                case ValueType::INT16:
                    if (wide_val < INT16_MIN || wide_val > INT16_MAX) {
                        return Value{std::monostate{}};
                    }
                    return Value{static_cast<int16_t>(wide_val)};

                case ValueType::INT32:
                    if (wide_val < INT32_MIN || wide_val > INT32_MAX) {
                        return Value{std::monostate{}};
                    }
                    return Value{static_cast<int32_t>(wide_val)};

                case ValueType::INT64:
                    return Value{wide_val};

                default:
                    return Value{std::monostate{}};
            }
        }

        // Unsigned integer conversions
        else if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> ||
                           std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>) {
            uint64_t wide_val = static_cast<uint64_t>(val);

            switch (target_type) {
                case ValueType::UINT8:
                    if (wide_val > UINT8_MAX) {
                        return Value{std::monostate{}};
                    }
                    return Value{static_cast<uint8_t>(wide_val)};

                case ValueType::UINT16:
                    if (wide_val > UINT16_MAX) {
                        return Value{std::monostate{}};
                    }
                    return Value{static_cast<uint16_t>(wide_val)};

                case ValueType::UINT32:
                    if (wide_val > UINT32_MAX) {
                        return Value{std::monostate{}};
                    }
                    return Value{static_cast<uint32_t>(wide_val)};

                case ValueType::UINT64:
                    return Value{wide_val};

                default:
                    return Value{std::monostate{}};
            }
        }

        // Floating point conversions
        else if constexpr (std::is_same_v<T, float>) {
            if (target_type == ValueType::DOUBLE) {
                return Value{static_cast<double>(val)};
            }
            return Value{std::monostate{}};
        }
        else if constexpr (std::is_same_v<T, double>) {
            if (target_type == ValueType::FLOAT) {
                return Value{static_cast<float>(val)};
            }
            return Value{std::monostate{}};
        }

        // Array conversions - signed integers
        else if constexpr (std::is_same_v<T, std::vector<int8_t>> ||
                           std::is_same_v<T, std::vector<int16_t>> ||
                           std::is_same_v<T, std::vector<int32_t>> ||
                           std::is_same_v<T, std::vector<int64_t>>) {

            switch (target_type) {
                case ValueType::INT8_ARRAY: {
                    std::vector<int8_t> result;
                    result.reserve(val.size());
                    for (auto v : val) {
                        int64_t wide = static_cast<int64_t>(v);
                        if (wide < INT8_MIN || wide > INT8_MAX) {
                            return Value{std::monostate{}};
                        }
                        result.push_back(static_cast<int8_t>(wide));
                    }
                    return Value{std::move(result)};
                }
                case ValueType::INT16_ARRAY: {
                    std::vector<int16_t> result;
                    result.reserve(val.size());
                    for (auto v : val) {
                        int64_t wide = static_cast<int64_t>(v);
                        if (wide < INT16_MIN || wide > INT16_MAX) {
                            return Value{std::monostate{}};
                        }
                        result.push_back(static_cast<int16_t>(wide));
                    }
                    return Value{std::move(result)};
                }
                case ValueType::INT32_ARRAY: {
                    std::vector<int32_t> result;
                    result.reserve(val.size());
                    for (auto v : val) {
                        int64_t wide = static_cast<int64_t>(v);
                        if (wide < INT32_MIN || wide > INT32_MAX) {
                            return Value{std::monostate{}};
                        }
                        result.push_back(static_cast<int32_t>(wide));
                    }
                    return Value{std::move(result)};
                }
                case ValueType::INT64_ARRAY: {
                    std::vector<int64_t> result;
                    result.reserve(val.size());
                    for (auto v : val) {
                        result.push_back(static_cast<int64_t>(v));
                    }
                    return Value{std::move(result)};
                }
                default:
                    return Value{std::monostate{}};
            }
        }

        // Array conversions - unsigned integers
        else if constexpr (std::is_same_v<T, std::vector<uint8_t>> ||
                           std::is_same_v<T, std::vector<uint16_t>> ||
                           std::is_same_v<T, std::vector<uint32_t>> ||
                           std::is_same_v<T, std::vector<uint64_t>>) {

            switch (target_type) {
                case ValueType::UINT8_ARRAY: {
                    std::vector<uint8_t> result;
                    result.reserve(val.size());
                    for (auto v : val) {
                        uint64_t wide = static_cast<uint64_t>(v);
                        if (wide > UINT8_MAX) {
                            return Value{std::monostate{}};
                        }
                        result.push_back(static_cast<uint8_t>(wide));
                    }
                    return Value{std::move(result)};
                }
                case ValueType::UINT16_ARRAY: {
                    std::vector<uint16_t> result;
                    result.reserve(val.size());
                    for (auto v : val) {
                        uint64_t wide = static_cast<uint64_t>(v);
                        if (wide > UINT16_MAX) {
                            return Value{std::monostate{}};
                        }
                        result.push_back(static_cast<uint16_t>(wide));
                    }
                    return Value{std::move(result)};
                }
                case ValueType::UINT32_ARRAY: {
                    std::vector<uint32_t> result;
                    result.reserve(val.size());
                    for (auto v : val) {
                        uint64_t wide = static_cast<uint64_t>(v);
                        if (wide > UINT32_MAX) {
                            return Value{std::monostate{}};
                        }
                        result.push_back(static_cast<uint32_t>(wide));
                    }
                    return Value{std::move(result)};
                }
                case ValueType::UINT64_ARRAY: {
                    std::vector<uint64_t> result;
                    result.reserve(val.size());
                    for (auto v : val) {
                        result.push_back(static_cast<uint64_t>(v));
                    }
                    return Value{std::move(result)};
                }
                default:
                    return Value{std::monostate{}};
            }
        }

        // Array conversions - floating point
        else if constexpr (std::is_same_v<T, std::vector<float>>) {
            if (target_type == ValueType::DOUBLE_ARRAY) {
                std::vector<double> result;
                result.reserve(val.size());
                for (float v : val) {
                    result.push_back(static_cast<double>(v));
                }
                return Value{std::move(result)};
            }
            return Value{std::monostate{}};
        }
        else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (target_type == ValueType::FLOAT_ARRAY) {
                std::vector<float> result;
                result.reserve(val.size());
                for (double v : val) {
                    result.push_back(static_cast<float>(v));
                }
                return Value{std::move(result)};
            }
            return Value{std::monostate{}};
        }

        // No conversion available
        else {
            return Value{std::monostate{}};
        }
    }, value);
}

} // namespace vss::types
