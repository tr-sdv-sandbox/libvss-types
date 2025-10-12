/**
 * @file struct_example.cpp
 * @brief Struct usage example for libvss-types (VSS 4.0)
 */

#include <vss/types/types.hpp>
#include <iostream>

using namespace vss::types;

int main() {
    std::cout << "libvss-types Struct Example\n\n";

    // ========================================================================
    // Example 1: Define a struct type
    // ========================================================================
    std::cout << "=== Example 1: Define Struct Types ===\n";

    StructRegistry registry;

    // Define DeliveryInfo struct
    StructDefinition delivery_info("DeliveryInfo", "Delivery information");
    delivery_info.add_field(FieldDefinition("Address", ValueType::STRING, "Destination address"))
                 .add_field(FieldDefinition("Receiver", ValueType::STRING, "Name of receiver"))
                 .add_field(FieldDefinition("Priority", ValueType::INT32, "Delivery priority"));

    registry.register_struct(std::move(delivery_info));
    std::cout << "Registered struct: DeliveryInfo\n";

    // Define Position struct
    StructDefinition position("Position", "Geographic position");
    position.add_field(FieldDefinition("Latitude", ValueType::DOUBLE, "Latitude in degrees"))
            .add_field(FieldDefinition("Longitude", ValueType::DOUBLE, "Longitude in degrees"))
            .add_field(FieldDefinition("Altitude", ValueType::DOUBLE, "Altitude in meters"));

    registry.register_struct(std::move(position));
    std::cout << "Registered struct: Position\n\n";

    // ========================================================================
    // Example 2: Create and populate struct values
    // ========================================================================
    std::cout << "=== Example 2: Create Struct Values ===\n";

    StructValue delivery{"DeliveryInfo"};
    delivery.set_field("Address", std::string("123 Main St, Anytown"));
    delivery.set_field("Receiver", std::string("John Doe"));
    delivery.set_field("Priority", int32_t(5));

    std::cout << "Created struct: " << delivery.type_name() << "\n";
    std::cout << "Fields:\n";
    for (const auto& [name, value] : delivery.fields()) {
        std::cout << "  " << name << ": " << value_type_to_string(get_value_type(value)) << "\n";
    }
    std::cout << "\n";

    // ========================================================================
    // Example 3: Validate struct against definition
    // ========================================================================
    std::cout << "=== Example 3: Validate Structs ===\n";

    auto validation_result = validate_struct(delivery, registry);
    if (validation_result.has_value()) {
        std::cout << "Validation failed: " << *validation_result << "\n";
    } else {
        std::cout << "✓ Struct is valid\n";
    }

    // Try invalid struct
    StructValue invalid{"DeliveryInfo"};
    invalid.set_field("Address", std::string("456 Oak Ave"));
    // Missing "Receiver" field

    validation_result = validate_struct(invalid, registry);
    if (validation_result.has_value()) {
        std::cout << "✗ Invalid struct: " << *validation_result << "\n";
    }
    std::cout << "\n";

    // ========================================================================
    // Example 4: Struct with defaults
    // ========================================================================
    std::cout << "=== Example 4: Struct with Default Values ===\n";

    StructDefinition config("Config", "Configuration struct");
    FieldDefinition timeout_field("Timeout", ValueType::INT32, "Timeout in seconds");
    timeout_field.default_value = int32_t(30);

    FieldDefinition enabled_field("Enabled", ValueType::BOOL, "Feature enabled");
    enabled_field.default_value = true;

    config.add_field(timeout_field).add_field(enabled_field);
    registry.register_struct(std::move(config));

    auto default_config = create_default_struct("Config", registry);
    if (default_config.has_value()) {
        std::cout << "Created Config with defaults:\n";
        const auto* timeout = default_config->get_field("Timeout");
        const auto* enabled = default_config->get_field("Enabled");

        if (timeout) std::cout << "  Timeout: " << std::get<int32_t>(*timeout) << "s\n";
        if (enabled) std::cout << "  Enabled: " << (std::get<bool>(*enabled) ? "yes" : "no") << "\n";
    }
    std::cout << "\n";

    // ========================================================================
    // Example 5: Use struct in Value variant
    // ========================================================================
    std::cout << "=== Example 5: Struct in Value Variant ===\n";

    auto pos_struct = std::make_shared<StructValue>("Position");
    pos_struct->set_field("Latitude", 37.7749);
    pos_struct->set_field("Longitude", -122.4194);
    pos_struct->set_field("Altitude", 16.0);

    Value position_value = pos_struct;
    std::cout << "Value type: " << value_type_to_string(get_value_type(position_value)) << "\n";

    if (auto struct_ptr = std::get_if<std::shared_ptr<StructValue>>(&position_value)) {
        std::cout << "Struct type: " << (*struct_ptr)->type_name() << "\n";
        std::cout << "Field count: " << (*struct_ptr)->fields().size() << "\n";
    }

    return 0;
}
