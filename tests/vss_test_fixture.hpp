/**
 * @file vss_test_fixture.hpp
 * @brief Test fixture for parsing VSS JSON and validating libvss-types support
 *
 * NOTE: JSON parsing is ONLY used in tests to validate type system completeness.
 * The library itself remains backend-agnostic with no JSON dependency.
 * In production, struct definitions would come from runtime metadata (e.g., KUKSA broker).
 */

#pragma once

#include <vss/types/types.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

// nlohmann/json - header-only library for test parsing
// Automatically fetched via CMake FetchContent or found system-wide
// This is ONLY for test validation, not part of the library
#include <nlohmann/json.hpp>

namespace vss::types::test {

using json = nlohmann::json;

/**
 * @brief VSS JSON parser for test validation
 *
 * Parses VSS JSON format and converts to StructRegistry.
 * This is test-only code to verify our type system supports full VSS spec.
 */
class VSSTestParser {
public:
    /**
     * @brief Parse VSS JSON file into StructRegistry
     *
     * @param json_path Path to VSS JSON file
     * @param registry Registry to populate
     * @return Error message if parsing fails, nullopt on success
     */
    static std::optional<std::string> parse_vss_json(
        const std::string& json_path,
        StructRegistry& registry)
    {
        try {
            // Read JSON file
            std::ifstream file(json_path);
            if (!file.is_open()) {
                return "Failed to open file: " + json_path;
            }

            json vss_json;
            file >> vss_json;

            // Parse Vehicle node
            if (!vss_json.contains("Vehicle")) {
                return "VSS JSON must contain 'Vehicle' root node";
            }

            // Recursively parse all structs
            parse_node(vss_json["Vehicle"], "Vehicle", registry);

            return std::nullopt;
        } catch (const std::exception& e) {
            return std::string("JSON parsing error: ") + e.what();
        }
    }

    /**
     * @brief Map VSS datatype string to ValueType enum
     */
    static std::optional<ValueType> vss_datatype_to_value_type(const std::string& datatype) {
        static const std::map<std::string, ValueType> type_map = {
            {"boolean", ValueType::BOOL},
            {"int32", ValueType::INT32},
            {"uint32", ValueType::UINT32},
            {"int64", ValueType::INT64},
            {"uint64", ValueType::UINT64},
            {"float", ValueType::FLOAT},
            {"double", ValueType::DOUBLE},
            {"string", ValueType::STRING},
            {"boolean[]", ValueType::BOOL_ARRAY},
            {"int32[]", ValueType::INT32_ARRAY},
            {"uint32[]", ValueType::UINT32_ARRAY},
            {"int64[]", ValueType::INT64_ARRAY},
            {"uint64[]", ValueType::UINT64_ARRAY},
            {"float[]", ValueType::FLOAT_ARRAY},
            {"double[]", ValueType::DOUBLE_ARRAY},
            {"string[]", ValueType::STRING_ARRAY},
            {"struct", ValueType::STRUCT},
            {"struct[]", ValueType::STRUCT_ARRAY}
        };

        auto it = type_map.find(datatype);
        if (it != type_map.end()) {
            return it->second;
        }
        return std::nullopt;
    }

private:
    /**
     * @brief Recursively parse VSS nodes
     */
    static void parse_node(
        const json& node,
        const std::string& path,
        StructRegistry& registry)
    {
        if (!node.contains("type")) {
            return;
        }

        std::string node_type = node["type"];

        // If this is a struct, register it
        if (node_type == "struct") {
            parse_struct(node, path, registry);
        }

        // Recursively process children
        if (node.contains("children")) {
            for (auto& [child_name, child_node] : node["children"].items()) {
                std::string child_path = path + "." + child_name;
                parse_node(child_node, child_path, registry);
            }
        }
    }

    /**
     * @brief Parse a struct definition
     */
    static void parse_struct(
        const json& node,
        const std::string& path,
        StructRegistry& registry)
    {
        // Create struct definition
        std::string description = node.value("description", "");
        StructDefinition struct_def(path, description);

        // Parse fields from children
        if (node.contains("children")) {
            for (auto& [field_name, field_node] : node["children"].items()) {
                if (!field_node.contains("datatype")) {
                    continue; // Skip nodes without datatype
                }

                std::string datatype_str = field_node["datatype"];
                auto value_type = vss_datatype_to_value_type(datatype_str);

                if (!value_type.has_value()) {
                    // Unknown type, skip
                    continue;
                }

                FieldDefinition field(
                    field_name,
                    *value_type,
                    field_node.value("description", "")
                );

                // Handle nested struct references
                if (*value_type == ValueType::STRUCT || *value_type == ValueType::STRUCT_ARRAY) {
                    if (field_node.contains("struct_type")) {
                        field.struct_type_name = field_node["struct_type"];
                    }
                }

                struct_def.add_field(std::move(field));
            }
        }

        // Register the struct
        registry.register_struct(std::move(struct_def));
    }
};

/**
 * @brief Helper to load test VSS configuration
 */
inline std::optional<std::string> load_test_vss_config(StructRegistry& registry) {
    // Look for vss_test.json in current directory or tests directory
    std::vector<std::string> search_paths = {
        "vss_test.json",
        "tests/vss_test.json",
        "../tests/vss_test.json"
    };

    for (const auto& path : search_paths) {
        std::ifstream test(path);
        if (test.good()) {
            return VSSTestParser::parse_vss_json(path, registry);
        }
    }

    return "Could not find vss_test.json in any search path";
}

} // namespace vss::types::test
