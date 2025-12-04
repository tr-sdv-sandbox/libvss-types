/**
 * @file quality.cpp
 * @brief Implementation of signal quality utilities
 */

#include <vss/types/quality.hpp>
#include <vss/types/value.hpp>
#include <algorithm>
#include <cctype>

namespace vss::types {

const char* signal_quality_to_string(SignalQuality quality) {
    switch (quality) {
        case SignalQuality::UNKNOWN:       return "UNKNOWN";
        case SignalQuality::VALID:         return "VALID";
        case SignalQuality::INVALID:       return "INVALID";
        case SignalQuality::NOT_AVAILABLE: return "NOT_AVAILABLE";
        default:                           return "UNKNOWN";
    }
}

std::optional<SignalQuality> signal_quality_from_string(const std::string& str) {
    // Convert to uppercase for case-insensitive matching
    std::string upper = str;
    std::transform(upper.begin(), upper.end(), upper.begin(),
                   [](unsigned char c){ return std::toupper(c); });

    if (upper == "UNKNOWN") return SignalQuality::UNKNOWN;
    if (upper == "VALID") return SignalQuality::VALID;
    if (upper == "INVALID") return SignalQuality::INVALID;
    if (upper == "NOT_AVAILABLE" || upper == "NOTAVAILABLE" || upper == "N/A")
        return SignalQuality::NOT_AVAILABLE;

    return std::nullopt;
}

bool dynamic_qualified_values_equal(const DynamicQualifiedValue& a, const DynamicQualifiedValue& b) {
    return a.quality == b.quality && values_equal(a.value, b.value);
}

bool dynamic_qualified_value_changed_beyond_threshold(
    const DynamicQualifiedValue& old_val,
    const DynamicQualifiedValue& new_val,
    double threshold) {

    // Quality change always counts as a change
    if (old_val.quality != new_val.quality) {
        return true;
    }

    // Delegate to value comparison with threshold
    return value_changed_beyond_threshold(old_val.value, new_val.value, threshold);
}

DynamicQualifiedValue convert_qualified_value_type(
    const DynamicQualifiedValue& qvalue,
    ValueType target_type
) {
    // If quality is not VALID, don't attempt conversion
    // Just preserve quality and timestamp, but don't convert value
    if (qvalue.quality != SignalQuality::VALID) {
        return qvalue;
    }

    // If value is empty, return as-is
    if (is_empty(qvalue.value)) {
        return qvalue;
    }

    // Attempt conversion
    Value converted = convert_value_type(qvalue.value, target_type);

    // Check if conversion succeeded
    if (is_empty(converted)) {
        // Conversion failed - return with INVALID quality
        return DynamicQualifiedValue{
            Value{std::monostate{}},
            SignalQuality::INVALID,
            qvalue.timestamp
        };
    }

    // Conversion succeeded - return with original quality preserved
    return DynamicQualifiedValue{
        std::move(converted),
        qvalue.quality,
        qvalue.timestamp
    };
}

} // namespace vss::types
