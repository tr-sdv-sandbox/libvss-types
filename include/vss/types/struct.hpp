/**
 * @file struct.hpp
 * @brief VSS 4.0 struct type support
 *
 * Defines structures for representing VSS structs at runtime.
 * Supports:
 * - Struct definitions (schema/type information)
 * - Struct values (instances with field data)
 * - Nested structs
 * - Arrays of structs
 */

#pragma once

#include "value.hpp"
#include <string>
#include <map>
#include <vector>
#include <optional>
#include <memory>

namespace vss::types {

/**
 * @brief Field definition within a struct type
 *
 * Describes a single field in a struct, including its type and metadata.
 */
struct FieldDefinition {
    std::string name;           ///< Field name (e.g., "address", "latitude")
    ValueType type;             ///< Field value type
    std::string description;    ///< Human-readable description
    std::optional<Value> default_value;  ///< Default value if not specified

    // For nested structs
    std::string struct_type_name;  ///< If type is STRUCT, the struct type name

    FieldDefinition() = default;

    FieldDefinition(std::string name, ValueType type)
        : name(std::move(name)), type(type) {}

    FieldDefinition(std::string name, ValueType type, std::string desc)
        : name(std::move(name)), type(type), description(std::move(desc)) {}
};

/**
 * @brief Definition of a VSS struct type (schema)
 *
 * Represents the structure/schema of a VSS struct type, not an instance.
 * This is the type information that would be loaded from VSS specification.
 *
 * Example VSS struct:
 * @code{.yaml}
 * DeliveryInfo:
 *   type: struct
 *   description: "Delivery information"
 *
 * DeliveryInfo.Address:
 *   datatype: string
 *   type: property
 *
 * DeliveryInfo.Receiver:
 *   datatype: string
 *   type: property
 * @endcode
 *
 * Becomes:
 * @code
 * StructDefinition delivery_info{
 *     "DeliveryInfo",
 *     "Delivery information",
 *     {
 *         {"Address", FieldDefinition{"Address", ValueType::STRING}},
 *         {"Receiver", FieldDefinition{"Receiver", ValueType::STRING}}
 *     }
 * };
 * @endcode
 */
class StructDefinition {
public:
    StructDefinition() = default;

    StructDefinition(std::string type_name, std::string description = "")
        : type_name_(std::move(type_name))
        , description_(std::move(description)) {}

    /**
     * @brief Get the struct type name
     * @return Type name (e.g., "DeliveryInfo", "Position")
     */
    const std::string& type_name() const noexcept { return type_name_; }

    /**
     * @brief Get struct description
     * @return Human-readable description
     */
    const std::string& description() const noexcept { return description_; }

    /**
     * @brief Get all field definitions
     * @return Map of field_name → FieldDefinition
     */
    const std::map<std::string, FieldDefinition>& fields() const noexcept { return fields_; }

    /**
     * @brief Add a field to the struct definition
     *
     * @param field The field definition to add
     * @return Reference to this for chaining
     */
    StructDefinition& add_field(FieldDefinition field);

    /**
     * @brief Get a specific field definition
     *
     * @param field_name Name of the field
     * @return Pointer to field definition, or nullptr if not found
     */
    const FieldDefinition* get_field(const std::string& field_name) const;

    /**
     * @brief Check if struct has a specific field
     *
     * @param field_name Name of the field
     * @return true if field exists, false otherwise
     */
    bool has_field(const std::string& field_name) const;

private:
    std::string type_name_;     ///< Struct type name
    std::string description_;   ///< Human-readable description
    std::map<std::string, FieldDefinition> fields_;  ///< Field name → definition
};

/**
 * @brief Runtime instance of a VSS struct (actual data)
 *
 * Represents an actual struct value with field data.
 * Must conform to a StructDefinition (validated separately).
 *
 * Example:
 * @code
 * StructValue delivery;
 * delivery.set_type_name("DeliveryInfo");
 * delivery.set_field("Address", std::string("123 Main St"));
 * delivery.set_field("Receiver", std::string("John Doe"));
 * @endcode
 */
class StructValue {
public:
    StructValue() = default;

    explicit StructValue(std::string type_name)
        : type_name_(std::move(type_name)) {}

    /**
     * @brief Get the struct type name
     * @return Type name (e.g., "DeliveryInfo")
     */
    const std::string& type_name() const noexcept { return type_name_; }

    /**
     * @brief Set the struct type name
     * @param type_name The type name
     */
    void set_type_name(std::string type_name) {
        type_name_ = std::move(type_name);
    }

    /**
     * @brief Get all field values
     * @return Map of field_name → Value
     */
    const std::map<std::string, Value>& fields() const noexcept { return fields_; }

    /**
     * @brief Set a field value
     *
     * @param field_name Name of the field
     * @param value Value to set
     */
    void set_field(std::string field_name, Value value);

    /**
     * @brief Get a field value
     *
     * @param field_name Name of the field
     * @return Pointer to value, or nullptr if field not set
     */
    const Value* get_field(const std::string& field_name) const;

    /**
     * @brief Check if a field is set
     *
     * @param field_name Name of the field
     * @return true if field has a value, false otherwise
     */
    bool has_field(const std::string& field_name) const;

    /**
     * @brief Remove a field
     *
     * @param field_name Name of the field to remove
     * @return true if field was removed, false if it didn't exist
     */
    bool remove_field(const std::string& field_name);

    /**
     * @brief Clear all fields
     */
    void clear();

private:
    std::string type_name_;               ///< Struct type name
    std::map<std::string, Value> fields_; ///< Field name → value
};

/**
 * @brief Registry of struct type definitions
 *
 * Manages the collection of known struct types, typically loaded from
 * VSS specification or vss-tools output.
 *
 * Thread-safe for read operations after initialization.
 */
class StructRegistry {
public:
    /**
     * @brief Register a struct type definition
     *
     * @param definition The struct definition to register
     * @return true if registered successfully, false if type already exists
     */
    bool register_struct(StructDefinition definition);

    /**
     * @brief Get a struct definition by type name
     *
     * @param type_name The struct type name
     * @return Pointer to definition, or nullptr if not found
     */
    const StructDefinition* get_struct(const std::string& type_name) const;

    /**
     * @brief Check if a struct type is registered
     *
     * @param type_name The struct type name
     * @return true if registered, false otherwise
     */
    bool has_struct(const std::string& type_name) const;

    /**
     * @brief Get all registered struct definitions
     *
     * @return Map of type_name → StructDefinition
     */
    const std::map<std::string, StructDefinition>& all_structs() const noexcept {
        return structs_;
    }

    /**
     * @brief Clear all registered struct definitions
     */
    void clear();

private:
    std::map<std::string, StructDefinition> structs_;
};

/**
 * @brief Validate a struct value against its definition
 *
 * Checks that:
 * - Type name matches a registered definition
 * - All required fields are present
 * - Field types match definition
 * - No extra fields exist (strict mode)
 *
 * @param value The struct value to validate
 * @param registry Registry containing struct definitions
 * @param strict If true, reject extra fields not in definition
 * @return nullopt if valid, error message otherwise
 */
std::optional<std::string> validate_struct(
    const StructValue& value,
    const StructRegistry& registry,
    bool strict = true
);

/**
 * @brief Create a struct value from a definition with default values
 *
 * Creates a struct instance with all default values populated.
 *
 * @param type_name The struct type name
 * @param registry Registry containing struct definitions
 * @return Struct value with defaults, or nullopt if type not found
 */
std::optional<StructValue> create_default_struct(
    const std::string& type_name,
    const StructRegistry& registry
);

} // namespace vss::types
