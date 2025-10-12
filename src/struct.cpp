/**
 * @file struct.cpp
 * @brief Implementation of struct support
 */

#include <vss/types/struct.hpp>
#include <sstream>

namespace vss::types {

// StructDefinition implementation

StructDefinition& StructDefinition::add_field(FieldDefinition field) {
    fields_[field.name] = std::move(field);
    return *this;
}

const FieldDefinition* StructDefinition::get_field(const std::string& field_name) const {
    auto it = fields_.find(field_name);
    return (it != fields_.end()) ? &it->second : nullptr;
}

bool StructDefinition::has_field(const std::string& field_name) const {
    return fields_.find(field_name) != fields_.end();
}

// StructValue implementation

void StructValue::set_field(std::string field_name, Value value) {
    fields_[std::move(field_name)] = std::move(value);
}

const Value* StructValue::get_field(const std::string& field_name) const {
    auto it = fields_.find(field_name);
    return (it != fields_.end()) ? &it->second : nullptr;
}

bool StructValue::has_field(const std::string& field_name) const {
    return fields_.find(field_name) != fields_.end();
}

bool StructValue::remove_field(const std::string& field_name) {
    return fields_.erase(field_name) > 0;
}

void StructValue::clear() {
    fields_.clear();
}

// StructRegistry implementation

bool StructRegistry::register_struct(StructDefinition definition) {
    const auto& type_name = definition.type_name();
    if (structs_.find(type_name) != structs_.end()) {
        return false;  // Already registered
    }
    structs_[type_name] = std::move(definition);
    return true;
}

const StructDefinition* StructRegistry::get_struct(const std::string& type_name) const {
    auto it = structs_.find(type_name);
    return (it != structs_.end()) ? &it->second : nullptr;
}

bool StructRegistry::has_struct(const std::string& type_name) const {
    return structs_.find(type_name) != structs_.end();
}

void StructRegistry::clear() {
    structs_.clear();
}

// Validation and utility functions

std::optional<std::string> validate_struct(
    const StructValue& value,
    const StructRegistry& registry,
    bool strict)
{
    // Check if struct type is registered
    const auto* definition = registry.get_struct(value.type_name());
    if (!definition) {
        return "Struct type '" + value.type_name() + "' not found in registry";
    }

    // Check all required fields from definition are present
    for (const auto& [field_name, field_def] : definition->fields()) {
        if (!value.has_field(field_name)) {
            // Check if field has a default value
            if (!field_def.default_value.has_value()) {
                return "Required field '" + field_name + "' missing in struct '" +
                       value.type_name() + "'";
            }
        } else {
            // Validate field type
            const auto* field_value = value.get_field(field_name);
            if (field_value) {
                ValueType actual_type = get_value_type(*field_value);
                if (!are_types_compatible(field_def.type, actual_type)) {
                    std::ostringstream oss;
                    oss << "Field '" << field_name << "' in struct '" << value.type_name()
                        << "' has type " << value_type_to_string(actual_type)
                        << " but expected " << value_type_to_string(field_def.type);
                    return oss.str();
                }

                // If field is a nested struct, validate recursively
                if (field_def.type == ValueType::STRUCT) {
                    if (auto struct_ptr = std::get_if<std::shared_ptr<StructValue>>(field_value)) {
                        if (*struct_ptr) {
                            auto nested_error = validate_struct(**struct_ptr, registry, strict);
                            if (nested_error.has_value()) {
                                return "Nested struct field '" + field_name + "': " + *nested_error;
                            }
                        }
                    }
                }
            }
        }
    }

    // In strict mode, check for extra fields not in definition
    if (strict) {
        for (const auto& [field_name, _] : value.fields()) {
            if (!definition->has_field(field_name)) {
                return "Extra field '" + field_name + "' not defined in struct type '" +
                       value.type_name() + "'";
            }
        }
    }

    return std::nullopt;  // Valid
}

std::optional<StructValue> create_default_struct(
    const std::string& type_name,
    const StructRegistry& registry)
{
    const auto* definition = registry.get_struct(type_name);
    if (!definition) {
        return std::nullopt;
    }

    StructValue value{type_name};

    // Set all default values
    for (const auto& [field_name, field_def] : definition->fields()) {
        if (field_def.default_value.has_value()) {
            value.set_field(field_name, *field_def.default_value);
        }
    }

    return value;
}

} // namespace vss::types
