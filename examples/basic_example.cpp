/**
 * @file basic_example.cpp
 * @brief Basic usage example for libvss-types
 */

#include <vss/types/types.hpp>
#include <iostream>

using namespace vss::types;

int main() {
    std::cout << "libvss-types v" << Version::string() << "\n\n";

    // ========================================================================
    // Example 1: Basic value types
    // ========================================================================
    std::cout << "=== Example 1: Basic Value Types ===\n";

    Value speed = 120.5f;
    Value rpm = int32_t(3000);
    Value door_locked = true;

    std::cout << "Speed type: " << value_type_to_string(get_value_type(speed)) << "\n";
    std::cout << "RPM type: " << value_type_to_string(get_value_type(rpm)) << "\n";
    std::cout << "Door locked type: " << value_type_to_string(get_value_type(door_locked)) << "\n\n";

    // ========================================================================
    // Example 2: Array types
    // ========================================================================
    std::cout << "=== Example 2: Array Types ===\n";

    Value tire_pressures = std::vector<float>{2.1f, 2.2f, 2.1f, 2.2f};
    std::cout << "Tire pressures type: " << value_type_to_string(get_value_type(tire_pressures)) << "\n\n";

    // ========================================================================
    // Example 3: Qualified values (with quality indicators)
    // ========================================================================
    std::cout << "=== Example 3: Qualified Values ===\n";

    QualifiedValue<float> temperature{22.5f, SignalQuality::VALID};
    std::cout << "Temperature: " << *temperature.value << "°C\n";
    std::cout << "Quality: " << signal_quality_to_string(temperature.quality) << "\n";
    std::cout << "Is valid: " << (temperature.is_valid() ? "yes" : "no") << "\n\n";

    QualifiedValue<float> broken_sensor{0.0f, SignalQuality::INVALID};
    std::cout << "Broken sensor quality: " << signal_quality_to_string(broken_sensor.quality) << "\n";
    std::cout << "Is invalid: " << (broken_sensor.is_invalid() ? "yes" : "no") << "\n\n";

    // ========================================================================
    // Example 4: Type compatibility
    // ========================================================================
    std::cout << "=== Example 4: Type Compatibility ===\n";

    std::cout << "float ↔ double: "
              << (are_types_compatible(ValueType::FLOAT, ValueType::DOUBLE) ? "compatible" : "incompatible") << "\n";
    std::cout << "int32 ↔ int64: "
              << (are_types_compatible(ValueType::INT32, ValueType::INT64) ? "compatible" : "incompatible") << "\n";
    std::cout << "float ↔ int32: "
              << (are_types_compatible(ValueType::FLOAT, ValueType::INT32) ? "compatible" : "incompatible") << "\n\n";

    // ========================================================================
    // Example 5: Dynamic qualified values
    // ========================================================================
    std::cout << "=== Example 5: Dynamic Qualified Values ===\n";

    DynamicQualifiedValue dyn_value{Value{42}};
    std::cout << "Dynamic value type: " << value_type_to_string(get_value_type(dyn_value.value)) << "\n";
    std::cout << "Value: " << std::get<int>(dyn_value.value) << "\n";
    std::cout << "Quality: " << signal_quality_to_string(dyn_value.quality) << "\n";

    return 0;
}
