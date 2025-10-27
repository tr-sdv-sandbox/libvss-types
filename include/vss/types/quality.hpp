/**
 * @file quality.hpp
 * @brief Signal quality and status types
 *
 * Defines signal quality indicators and status information inspired by
 * libVSSDAG's invalid/unavailable signal handling and automotive standards
 * (AUTOSAR, DDS QoS).
 */

#pragma once

#include "value.hpp"
#include <chrono>
#include <optional>
#include <stdexcept>

namespace vss::types {

/**
 * @brief Signal quality/validity status
 *
 * Indicates the quality and validity of a signal value.
 * Supports the sophisticated error handling from libVSSDAG.
 */
enum class SignalQuality {
    /**
     * @brief Unknown quality (not specified)
     */
    UNKNOWN = 0,

    /**
     * @brief Signal value is valid and reliable
     */
    VALID = 1,

    /**
     * @brief Signal value is invalid
     *
     * Sensor error, out of range, failed validation, etc.
     * Value should not be trusted.
     */
    INVALID = 2,

    /**
     * @brief Signal is not available
     *
     * Sensor not present, disconnected, or not yet initialized.
     * Different from INVALID - not an error, just unavailable.
     */
    NOT_AVAILABLE = 3
};

/**
 * @brief Convert SignalQuality to string
 *
 * @param quality The quality enum
 * @return String representation
 */
const char* signal_quality_to_string(SignalQuality quality);

/**
 * @brief Parse SignalQuality from string
 *
 * @param str String representation (case-insensitive)
 * @return SignalQuality if recognized, std::nullopt otherwise
 */
std::optional<SignalQuality> signal_quality_from_string(const std::string& str);


/**
 * @brief A value with associated quality and timestamp
 *
 * Wraps a VSS value with metadata about its quality and timing.
 * This is the core type for sophisticated signal handling.
 *
 * Example:
 * @code
 * QualifiedValue<float> speed{
 *     120.5f,                  // value
 *     SignalQuality::VALID,    // quality
 *     std::chrono::system_clock::now()  // timestamp
 * };
 *
 * if (speed.is_valid()) {
 *     LOG(INFO) << "Speed: " << *speed.value();
 * }
 * @endcode
 */
template<typename T>
struct QualifiedValue {
    std::optional<T> value;       ///< The actual value (nullopt if no value)
    SignalQuality quality;        ///< Quality indicator
    std::chrono::system_clock::time_point timestamp;  ///< When value was set

    QualifiedValue()
        : quality(SignalQuality::UNKNOWN)
        , timestamp(std::chrono::system_clock::now()) {}

    explicit QualifiedValue(T val)
        : value(std::move(val))
        , quality(SignalQuality::VALID)
        , timestamp(std::chrono::system_clock::now()) {}

    QualifiedValue(T val, SignalQuality q)
        : value(std::move(val))
        , quality(q)
        , timestamp(std::chrono::system_clock::now()) {}

    QualifiedValue(T val, SignalQuality q, std::chrono::system_clock::time_point ts)
        : value(std::move(val))
        , quality(q)
        , timestamp(ts) {}

    /**
     * @brief Check if value is valid and present
     */
    bool is_valid() const {
        return value.has_value() && quality == SignalQuality::VALID;
    }

    /**
     * @brief Check if value is invalid (error state)
     */
    bool is_invalid() const {
        return quality == SignalQuality::INVALID;
    }

    /**
     * @brief Check if signal is not available
     */
    bool is_not_available() const {
        return quality == SignalQuality::NOT_AVAILABLE;
    }

    /**
     * @brief Get value or throw if not valid
     */
    const T& value_or_throw() const {
        if (!value.has_value()) {
            throw std::runtime_error("QualifiedValue has no value");
        }
        return *value;
    }

    /**
     * @brief Get value or default if not valid
     */
    T value_or(T default_value) const {
        return value.value_or(std::move(default_value));
    }

    /**
     * @brief Get age of the value
     */
    std::chrono::milliseconds age() const {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp);
    }
};

/**
 * @brief Dynamic qualified value (for runtime type usage)
 *
 * Like QualifiedValue<T> but uses Value variant instead of template parameter.
 * Useful when type is not known at compile time.
 */
struct DynamicQualifiedValue {
    Value value;                  ///< The actual value (may be empty)
    SignalQuality quality;        ///< Quality indicator
    std::chrono::system_clock::time_point timestamp;  ///< When value was set

    DynamicQualifiedValue()
        : quality(SignalQuality::UNKNOWN)
        , timestamp(std::chrono::system_clock::now()) {}

    explicit DynamicQualifiedValue(Value val)
        : value(std::move(val))
        , quality(SignalQuality::VALID)
        , timestamp(std::chrono::system_clock::now()) {}

    DynamicQualifiedValue(Value val, SignalQuality q)
        : value(std::move(val))
        , quality(q)
        , timestamp(std::chrono::system_clock::now()) {}

    DynamicQualifiedValue(Value val, SignalQuality q, std::chrono::system_clock::time_point ts)
        : value(std::move(val))
        , quality(q)
        , timestamp(ts) {}

    /**
     * @brief Check if value is valid and not empty
     */
    bool is_valid() const {
        return !is_empty(value) && quality == SignalQuality::VALID;
    }

    /**
     * @brief Check if value is invalid (error state)
     */
    bool is_invalid() const {
        return quality == SignalQuality::INVALID;
    }

    /**
     * @brief Check if signal is not available
     */
    bool is_not_available() const {
        return quality == SignalQuality::NOT_AVAILABLE;
    }

    /**
     * @brief Get age of the value
     */
    std::chrono::milliseconds age() const {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp);
    }
};

/**
 * @brief Convert DynamicQualifiedValue to a target type with quality preservation
 *
 * Attempts to convert the value to target_type while preserving timestamp.
 * Quality handling:
 * - If conversion succeeds: preserves original quality
 * - If types are incompatible: sets quality to INVALID
 * - If narrowing out of range: sets quality to INVALID
 * - If original quality was not VALID: preserves original quality, conversion not attempted
 *
 * @param qvalue Source qualified value
 * @param target_type Desired value type
 * @return Converted qualified value with appropriate quality
 */
DynamicQualifiedValue convert_qualified_value_type(
    const DynamicQualifiedValue& qvalue,
    ValueType target_type
);

} // namespace vss::types
