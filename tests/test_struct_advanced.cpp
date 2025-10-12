/**
 * @file test_struct_advanced.cpp
 * @brief Advanced struct tests - nested structs, struct arrays, complex scenarios
 */

#include <vss/types/struct.hpp>
#include <gtest/gtest.h>

using namespace vss::types;

TEST(StructAdvancedTest, NestedStructs) {
    StructRegistry registry;

    // Define inner struct: Position
    StructDefinition position("Position", "Geographic position");
    position.add_field(FieldDefinition("Latitude", ValueType::DOUBLE))
            .add_field(FieldDefinition("Longitude", ValueType::DOUBLE))
            .add_field(FieldDefinition("Altitude", ValueType::DOUBLE));
    registry.register_struct(std::move(position));

    // Define outer struct: DeliveryInfo with nested Position
    StructDefinition delivery_info("DeliveryInfo", "Delivery information");
    delivery_info.add_field(FieldDefinition("Address", ValueType::STRING));

    FieldDefinition location_field("Location", ValueType::STRUCT, "Delivery location");
    location_field.struct_type_name = "Position";
    delivery_info.add_field(location_field);

    delivery_info.add_field(FieldDefinition("Receiver", ValueType::STRING));
    registry.register_struct(std::move(delivery_info));

    // Create nested struct value
    auto pos = std::make_shared<StructValue>("Position");
    pos->set_field("Latitude", 37.7749);
    pos->set_field("Longitude", -122.4194);
    pos->set_field("Altitude", 16.0);

    StructValue delivery("DeliveryInfo");
    delivery.set_field("Address", std::string("123 Main St"));
    delivery.set_field("Location", Value{pos});
    delivery.set_field("Receiver", std::string("John Doe"));

    // Validate nested struct
    auto error = validate_struct(delivery, registry);
    EXPECT_FALSE(error.has_value()) << (error.has_value() ? *error : "");

    // Verify we can access nested fields
    const auto* location_value = delivery.get_field("Location");
    ASSERT_NE(location_value, nullptr);

    auto location_ptr = std::get<std::shared_ptr<StructValue>>(*location_value);
    ASSERT_NE(location_ptr, nullptr);
    EXPECT_EQ(location_ptr->type_name(), "Position");

    const auto* lat = location_ptr->get_field("Latitude");
    ASSERT_NE(lat, nullptr);
    EXPECT_DOUBLE_EQ(std::get<double>(*lat), 37.7749);
}

TEST(StructAdvancedTest, DeeplyNestedStructs) {
    StructRegistry registry;

    // Level 1: Coordinates
    StructDefinition coords("Coordinates");
    coords.add_field(FieldDefinition("X", ValueType::DOUBLE))
          .add_field(FieldDefinition("Y", ValueType::DOUBLE));
    registry.register_struct(std::move(coords));

    // Level 2: Position contains Coordinates
    StructDefinition position("Position");
    FieldDefinition coords_field("Coords", ValueType::STRUCT);
    coords_field.struct_type_name = "Coordinates";
    position.add_field(coords_field);
    position.add_field(FieldDefinition("Elevation", ValueType::DOUBLE));
    registry.register_struct(std::move(position));

    // Level 3: Location contains Position
    StructDefinition location("Location");
    FieldDefinition pos_field("Position", ValueType::STRUCT);
    pos_field.struct_type_name = "Position";
    location.add_field(pos_field);
    location.add_field(FieldDefinition("Name", ValueType::STRING));
    registry.register_struct(std::move(location));

    // Create deeply nested value
    auto coords_val = std::make_shared<StructValue>("Coordinates");
    coords_val->set_field("X", 10.5);
    coords_val->set_field("Y", 20.3);

    auto pos_val = std::make_shared<StructValue>("Position");
    pos_val->set_field("Coords", Value{coords_val});
    pos_val->set_field("Elevation", 500.0);

    StructValue loc_val("Location");
    loc_val.set_field("Position", Value{pos_val});
    loc_val.set_field("Name", std::string("Mountain Peak"));

    // Validate
    auto error = validate_struct(loc_val, registry);
    EXPECT_FALSE(error.has_value()) << (error.has_value() ? *error : "");
}

TEST(StructAdvancedTest, StructArrays) {
    StructRegistry registry;

    // Define struct type
    StructDefinition waypoint("Waypoint");
    waypoint.add_field(FieldDefinition("Latitude", ValueType::DOUBLE))
            .add_field(FieldDefinition("Longitude", ValueType::DOUBLE))
            .add_field(FieldDefinition("Name", ValueType::STRING));
    registry.register_struct(std::move(waypoint));

    // Create array of structs
    std::vector<std::shared_ptr<StructValue>> waypoints;

    auto wp1 = std::make_shared<StructValue>("Waypoint");
    wp1->set_field("Latitude", 37.7749);
    wp1->set_field("Longitude", -122.4194);
    wp1->set_field("Name", std::string("San Francisco"));
    waypoints.push_back(wp1);

    auto wp2 = std::make_shared<StructValue>("Waypoint");
    wp2->set_field("Latitude", 34.0522);
    wp2->set_field("Longitude", -118.2437);
    wp2->set_field("Name", std::string("Los Angeles"));
    waypoints.push_back(wp2);

    auto wp3 = std::make_shared<StructValue>("Waypoint");
    wp3->set_field("Latitude", 40.7128);
    wp3->set_field("Longitude", -74.0060);
    wp3->set_field("Name", std::string("New York"));
    waypoints.push_back(wp3);

    // Put in Value variant
    Value waypoint_array = waypoints;
    EXPECT_EQ(get_value_type(waypoint_array), ValueType::STRUCT_ARRAY);

    // Extract and verify
    auto& arr = std::get<std::vector<std::shared_ptr<StructValue>>>(waypoint_array);
    EXPECT_EQ(arr.size(), 3);

    EXPECT_EQ(arr[0]->type_name(), "Waypoint");
    EXPECT_EQ(arr[1]->type_name(), "Waypoint");
    EXPECT_EQ(arr[2]->type_name(), "Waypoint");

    const auto* name = arr[0]->get_field("Name");
    ASSERT_NE(name, nullptr);
    EXPECT_EQ(std::get<std::string>(*name), "San Francisco");
}

TEST(StructAdvancedTest, StructWithAllPrimitiveTypes) {
    StructRegistry registry;

    // Define struct with all primitive types
    StructDefinition all_types("AllTypes");
    all_types.add_field(FieldDefinition("BoolField", ValueType::BOOL))
             .add_field(FieldDefinition("Int32Field", ValueType::INT32))
             .add_field(FieldDefinition("Uint32Field", ValueType::UINT32))
             .add_field(FieldDefinition("Int64Field", ValueType::INT64))
             .add_field(FieldDefinition("Uint64Field", ValueType::UINT64))
             .add_field(FieldDefinition("FloatField", ValueType::FLOAT))
             .add_field(FieldDefinition("DoubleField", ValueType::DOUBLE))
             .add_field(FieldDefinition("StringField", ValueType::STRING));
    registry.register_struct(std::move(all_types));

    // Create struct value
    StructValue value("AllTypes");
    value.set_field("BoolField", true);
    value.set_field("Int32Field", int32_t(-42));
    value.set_field("Uint32Field", uint32_t(42));
    value.set_field("Int64Field", int64_t(-123456789));
    value.set_field("Uint64Field", uint64_t(987654321));
    value.set_field("FloatField", 3.14f);
    value.set_field("DoubleField", 2.71828);
    value.set_field("StringField", std::string("test"));

    // Validate
    auto error = validate_struct(value, registry);
    EXPECT_FALSE(error.has_value()) << (error.has_value() ? *error : "");

    // Verify all fields
    EXPECT_EQ(std::get<bool>(*value.get_field("BoolField")), true);
    EXPECT_EQ(std::get<int32_t>(*value.get_field("Int32Field")), -42);
    EXPECT_EQ(std::get<uint32_t>(*value.get_field("Uint32Field")), 42u);
    EXPECT_EQ(std::get<int64_t>(*value.get_field("Int64Field")), -123456789);
    EXPECT_EQ(std::get<uint64_t>(*value.get_field("Uint64Field")), 987654321u);
    EXPECT_FLOAT_EQ(std::get<float>(*value.get_field("FloatField")), 3.14f);
    EXPECT_DOUBLE_EQ(std::get<double>(*value.get_field("DoubleField")), 2.71828);
    EXPECT_EQ(std::get<std::string>(*value.get_field("StringField")), "test");
}

TEST(StructAdvancedTest, StructWithAllArrayTypes) {
    StructRegistry registry;

    // Define struct with all array types
    StructDefinition all_arrays("AllArrayTypes");
    all_arrays.add_field(FieldDefinition("BoolArray", ValueType::BOOL_ARRAY))
              .add_field(FieldDefinition("Int32Array", ValueType::INT32_ARRAY))
              .add_field(FieldDefinition("Uint32Array", ValueType::UINT32_ARRAY))
              .add_field(FieldDefinition("Int64Array", ValueType::INT64_ARRAY))
              .add_field(FieldDefinition("Uint64Array", ValueType::UINT64_ARRAY))
              .add_field(FieldDefinition("FloatArray", ValueType::FLOAT_ARRAY))
              .add_field(FieldDefinition("DoubleArray", ValueType::DOUBLE_ARRAY))
              .add_field(FieldDefinition("StringArray", ValueType::STRING_ARRAY));
    registry.register_struct(std::move(all_arrays));

    // Create struct value with arrays
    StructValue value("AllArrayTypes");
    value.set_field("BoolArray", Value{std::vector<bool>{true, false, true}});
    value.set_field("Int32Array", Value{std::vector<int32_t>{1, -2, 3}});
    value.set_field("Uint32Array", Value{std::vector<uint32_t>{10, 20, 30}});
    value.set_field("Int64Array", Value{std::vector<int64_t>{100, -200, 300}});
    value.set_field("Uint64Array", Value{std::vector<uint64_t>{1000, 2000, 3000}});
    value.set_field("FloatArray", Value{std::vector<float>{1.1f, 2.2f, 3.3f}});
    value.set_field("DoubleArray", Value{std::vector<double>{1.11, 2.22, 3.33}});
    value.set_field("StringArray", Value{std::vector<std::string>{"a", "b", "c"}});

    // Validate
    auto error = validate_struct(value, registry);
    EXPECT_FALSE(error.has_value()) << (error.has_value() ? *error : "");

    // Verify array contents
    auto& bool_arr = std::get<std::vector<bool>>(*value.get_field("BoolArray"));
    EXPECT_EQ(bool_arr.size(), 3);
    EXPECT_EQ(bool_arr[0], true);

    auto& int32_arr = std::get<std::vector<int32_t>>(*value.get_field("Int32Array"));
    EXPECT_EQ(int32_arr.size(), 3);
    EXPECT_EQ(int32_arr[1], -2);

    auto& string_arr = std::get<std::vector<std::string>>(*value.get_field("StringArray"));
    EXPECT_EQ(string_arr.size(), 3);
    EXPECT_EQ(string_arr[2], "c");
}

TEST(StructAdvancedTest, StructArrayInStruct) {
    StructRegistry registry;

    // Define Point struct
    StructDefinition point("Point");
    point.add_field(FieldDefinition("X", ValueType::DOUBLE))
         .add_field(FieldDefinition("Y", ValueType::DOUBLE));
    registry.register_struct(std::move(point));

    // Define Route struct containing array of Points
    StructDefinition route("Route");
    route.add_field(FieldDefinition("Name", ValueType::STRING));
    FieldDefinition waypoints_field("Waypoints", ValueType::STRUCT_ARRAY);
    waypoints_field.struct_type_name = "Point";
    route.add_field(waypoints_field);
    registry.register_struct(std::move(route));

    // Create route with multiple points
    std::vector<std::shared_ptr<StructValue>> points;

    auto p1 = std::make_shared<StructValue>("Point");
    p1->set_field("X", 0.0);
    p1->set_field("Y", 0.0);
    points.push_back(p1);

    auto p2 = std::make_shared<StructValue>("Point");
    p2->set_field("X", 10.5);
    p2->set_field("Y", 20.3);
    points.push_back(p2);

    auto p3 = std::make_shared<StructValue>("Point");
    p3->set_field("X", 30.1);
    p3->set_field("Y", 40.7);
    points.push_back(p3);

    StructValue route_val("Route");
    route_val.set_field("Name", std::string("Path A"));
    route_val.set_field("Waypoints", Value{points});

    // Validate
    auto error = validate_struct(route_val, registry);
    EXPECT_FALSE(error.has_value()) << (error.has_value() ? *error : "");

    // Verify nested array
    const auto* waypoints_val = route_val.get_field("Waypoints");
    ASSERT_NE(waypoints_val, nullptr);

    auto& waypoints_arr = std::get<std::vector<std::shared_ptr<StructValue>>>(*waypoints_val);
    EXPECT_EQ(waypoints_arr.size(), 3);

    const auto* x = waypoints_arr[1]->get_field("X");
    ASSERT_NE(x, nullptr);
    EXPECT_DOUBLE_EQ(std::get<double>(*x), 10.5);
}

TEST(StructAdvancedTest, InvalidNestedStructType) {
    StructRegistry registry;

    // Define only inner struct
    StructDefinition inner("Inner");
    inner.add_field(FieldDefinition("Value", ValueType::INT32));
    registry.register_struct(std::move(inner));

    // Create struct with wrong nested type
    StructValue outer("Outer");

    auto wrong_struct = std::make_shared<StructValue>("WrongType");
    wrong_struct->set_field("Value", 42);

    outer.set_field("Nested", Value{wrong_struct});

    // Note: This tests that we can create mismatched types
    // Full validation would require the outer struct definition
    EXPECT_NE(wrong_struct->type_name(), "Inner");
}
