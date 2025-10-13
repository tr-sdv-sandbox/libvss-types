/**
 * @file test_vss_integration.cpp
 * @brief Integration test: Validate libvss-types supports complete VSS specification
 *
 * This test parses a VSS JSON file and verifies that libvss-types can represent
 * all VSS data types including primitives, arrays, and structs (VSS 4.0).
 *
 * NOTE: JSON parsing is ONLY used in this test to validate type completeness.
 * The library itself has no JSON dependency - in production, struct definitions
 * would come from runtime metadata (e.g., KUKSA databroker).
 */

#include "vss_test_fixture.hpp"
#include <gtest/gtest.h>

using namespace vss::types;
using namespace vss::types::test;

class VSSIntegrationTest : public ::testing::Test {
protected:
    StructRegistry registry;

    void SetUp() override {
        // Load test VSS configuration
        auto error = load_test_vss_config(registry);
        ASSERT_FALSE(error.has_value()) << "Failed to load VSS config: " << *error;
    }
};

TEST_F(VSSIntegrationTest, AllPrimitiveTypesSupported) {
    // Verify that our ValueType enum supports all VSS primitive types

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("boolean").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("boolean"), ValueType::BOOL);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("int32").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("int32"), ValueType::INT32);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("uint32").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("uint32"), ValueType::UINT32);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("int64").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("int64"), ValueType::INT64);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("uint64").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("uint64"), ValueType::UINT64);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("float").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("float"), ValueType::FLOAT);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("double").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("double"), ValueType::DOUBLE);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("string").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("string"), ValueType::STRING);
}

TEST_F(VSSIntegrationTest, AllArrayTypesSupported) {
    // Verify that our ValueType enum supports all VSS array types

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("boolean[]").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("boolean[]"), ValueType::BOOL_ARRAY);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("int32[]").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("int32[]"), ValueType::INT32_ARRAY);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("uint32[]").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("uint32[]"), ValueType::UINT32_ARRAY);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("int64[]").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("int64[]"), ValueType::INT64_ARRAY);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("uint64[]").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("uint64[]"), ValueType::UINT64_ARRAY);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("float[]").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("float[]"), ValueType::FLOAT_ARRAY);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("double[]").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("double[]"), ValueType::DOUBLE_ARRAY);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("string[]").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("string[]"), ValueType::STRING_ARRAY);
}

TEST_F(VSSIntegrationTest, StructTypesSupported) {
    // Verify struct and struct array types are supported

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("struct").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("struct"), ValueType::STRUCT);

    EXPECT_TRUE(VSSTestParser::vss_datatype_to_value_type("struct[]").has_value());
    EXPECT_EQ(*VSSTestParser::vss_datatype_to_value_type("struct[]"), ValueType::STRUCT_ARRAY);
}

TEST_F(VSSIntegrationTest, PositionStructLoaded) {
    // Verify Position struct was loaded correctly
    ASSERT_TRUE(registry.has_struct("Vehicle.Test.Position"));

    const auto* position = registry.get_struct("Vehicle.Test.Position");
    ASSERT_NE(position, nullptr);

    EXPECT_EQ(position->type_name(), "Vehicle.Test.Position");
    EXPECT_FALSE(position->description().empty());

    // Check fields
    EXPECT_TRUE(position->has_field("Latitude"));
    EXPECT_TRUE(position->has_field("Longitude"));
    EXPECT_TRUE(position->has_field("Altitude"));

    const auto* lat_field = position->get_field("Latitude");
    ASSERT_NE(lat_field, nullptr);
    EXPECT_EQ(lat_field->type, ValueType::DOUBLE);

    const auto* lon_field = position->get_field("Longitude");
    ASSERT_NE(lon_field, nullptr);
    EXPECT_EQ(lon_field->type, ValueType::DOUBLE);

    const auto* alt_field = position->get_field("Altitude");
    ASSERT_NE(alt_field, nullptr);
    EXPECT_EQ(alt_field->type, ValueType::DOUBLE);
}

TEST_F(VSSIntegrationTest, DeliveryInfoStructLoaded) {
    // Verify DeliveryInfo struct with nested struct was loaded
    ASSERT_TRUE(registry.has_struct("Vehicle.Test.DeliveryInfo"));

    const auto* delivery_info = registry.get_struct("Vehicle.Test.DeliveryInfo");
    ASSERT_NE(delivery_info, nullptr);

    EXPECT_EQ(delivery_info->type_name(), "Vehicle.Test.DeliveryInfo");

    // Check primitive fields
    EXPECT_TRUE(delivery_info->has_field("Address"));
    EXPECT_TRUE(delivery_info->has_field("Receiver"));
    EXPECT_TRUE(delivery_info->has_field("Priority"));

    const auto* address_field = delivery_info->get_field("Address");
    ASSERT_NE(address_field, nullptr);
    EXPECT_EQ(address_field->type, ValueType::STRING);

    const auto* priority_field = delivery_info->get_field("Priority");
    ASSERT_NE(priority_field, nullptr);
    EXPECT_EQ(priority_field->type, ValueType::INT32);

    // Check nested struct field
    EXPECT_TRUE(delivery_info->has_field("Location"));
    const auto* location_field = delivery_info->get_field("Location");
    ASSERT_NE(location_field, nullptr);
    EXPECT_EQ(location_field->type, ValueType::STRUCT);
    EXPECT_EQ(location_field->struct_type_name, "Vehicle.Test.Position");
}

TEST_F(VSSIntegrationTest, RouteStructWithArrayLoaded) {
    // Verify Route struct with struct array was loaded
    ASSERT_TRUE(registry.has_struct("Vehicle.Test.Route"));

    const auto* route = registry.get_struct("Vehicle.Test.Route");
    ASSERT_NE(route, nullptr);

    EXPECT_TRUE(route->has_field("Name"));
    EXPECT_TRUE(route->has_field("Waypoints"));

    const auto* waypoints_field = route->get_field("Waypoints");
    ASSERT_NE(waypoints_field, nullptr);
    EXPECT_EQ(waypoints_field->type, ValueType::STRUCT_ARRAY);
    EXPECT_EQ(waypoints_field->struct_type_name, "Vehicle.Test.Waypoint");
}

TEST_F(VSSIntegrationTest, CreateAndValidatePositionInstance) {
    // Create an instance of Position struct and validate it
    ASSERT_TRUE(registry.has_struct("Vehicle.Test.Position"));

    StructValue position("Vehicle.Test.Position");
    position.set_field("Latitude", 37.7749);
    position.set_field("Longitude", -122.4194);
    position.set_field("Altitude", 16.0);

    // Validate against registry
    auto error = validate_struct(position, registry);
    EXPECT_FALSE(error.has_value()) << "Validation error: " << (error.has_value() ? *error : "");
}

TEST_F(VSSIntegrationTest, CreateAndValidateDeliveryInfoWithNestedStruct) {
    // Create DeliveryInfo instance with nested Position
    ASSERT_TRUE(registry.has_struct("Vehicle.Test.DeliveryInfo"));
    ASSERT_TRUE(registry.has_struct("Vehicle.Test.Position"));

    // Create nested Position
    auto position = std::make_shared<StructValue>("Vehicle.Test.Position");
    position->set_field("Latitude", 37.7749);
    position->set_field("Longitude", -122.4194);
    position->set_field("Altitude", 16.0);

    // Create DeliveryInfo with nested Position
    StructValue delivery("Vehicle.Test.DeliveryInfo");
    delivery.set_field("Address", std::string("123 Main St"));
    delivery.set_field("Receiver", std::string("John Doe"));
    delivery.set_field("Priority", int32_t(5));
    delivery.set_field("Location", Value{position});

    // Validate (should recursively validate nested struct)
    auto error = validate_struct(delivery, registry);
    EXPECT_FALSE(error.has_value()) << "Validation error: " << (error.has_value() ? *error : "");
}

TEST_F(VSSIntegrationTest, CreateAndValidateRouteWithStructArray) {
    // Create Route instance with array of Waypoint structs
    ASSERT_TRUE(registry.has_struct("Vehicle.Test.Route"));
    ASSERT_TRUE(registry.has_struct("Vehicle.Test.Waypoint"));

    // Create waypoints
    std::vector<std::shared_ptr<StructValue>> waypoints;

    auto wp1 = std::make_shared<StructValue>("Vehicle.Test.Waypoint");
    wp1->set_field("Latitude", 37.7749);
    wp1->set_field("Longitude", -122.4194);
    wp1->set_field("Name", std::string("San Francisco"));
    waypoints.push_back(wp1);

    auto wp2 = std::make_shared<StructValue>("Vehicle.Test.Waypoint");
    wp2->set_field("Latitude", 34.0522);
    wp2->set_field("Longitude", -118.2437);
    wp2->set_field("Name", std::string("Los Angeles"));
    waypoints.push_back(wp2);

    // Create Route
    StructValue route("Vehicle.Test.Route");
    route.set_field("Name", std::string("California Tour"));
    route.set_field("Waypoints", Value{waypoints});

    // Validate
    auto error = validate_struct(route, registry);
    EXPECT_FALSE(error.has_value()) << "Validation error: " << (error.has_value() ? *error : "");
}

TEST_F(VSSIntegrationTest, AllVSSTypesCanBeRepresented) {
    // Verify that Value variant can hold all VSS types

    // Primitives
    Value bool_val = true;
    EXPECT_EQ(get_value_type(bool_val), ValueType::BOOL);

    Value int32_val = int32_t(42);
    EXPECT_EQ(get_value_type(int32_val), ValueType::INT32);

    Value uint32_val = uint32_t(42);
    EXPECT_EQ(get_value_type(uint32_val), ValueType::UINT32);

    Value int64_val = int64_t(42);
    EXPECT_EQ(get_value_type(int64_val), ValueType::INT64);

    Value uint64_val = uint64_t(42);
    EXPECT_EQ(get_value_type(uint64_val), ValueType::UINT64);

    Value float_val = 3.14f;
    EXPECT_EQ(get_value_type(float_val), ValueType::FLOAT);

    Value double_val = 3.14159;
    EXPECT_EQ(get_value_type(double_val), ValueType::DOUBLE);

    Value string_val = std::string("test");
    EXPECT_EQ(get_value_type(string_val), ValueType::STRING);

    // Arrays
    Value bool_array = std::vector<bool>{true, false};
    EXPECT_EQ(get_value_type(bool_array), ValueType::BOOL_ARRAY);

    Value int32_array = std::vector<int32_t>{1, 2, 3};
    EXPECT_EQ(get_value_type(int32_array), ValueType::INT32_ARRAY);

    Value string_array = std::vector<std::string>{"a", "b", "c"};
    EXPECT_EQ(get_value_type(string_array), ValueType::STRING_ARRAY);

    // Struct
    auto struct_ptr = std::make_shared<StructValue>("Test");
    Value struct_val = struct_ptr;
    EXPECT_EQ(get_value_type(struct_val), ValueType::STRUCT);

    // Struct array
    std::vector<std::shared_ptr<StructValue>> struct_vec;
    struct_vec.push_back(struct_ptr);
    Value struct_array = struct_vec;
    EXPECT_EQ(get_value_type(struct_array), ValueType::STRUCT_ARRAY);
}
