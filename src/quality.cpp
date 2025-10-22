/**
 * @file quality.cpp
 * @brief Implementation of signal quality utilities
 */

#include <vss/types/quality.hpp>
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

} // namespace vss::types
