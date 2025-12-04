/**
 * @file test_value.cpp
 * @brief Basic tests for value types
 */

#include <vss/types/value.hpp>
#include <vss/types/struct.hpp>
#include <gtest/gtest.h>

using namespace vss::types;

TEST(ValueTest, PrimitiveTypes) {
    // Test all primitive types
    Value v_bool = true;
    Value v_int8 = int8_t(42);
    Value v_int16 = int16_t(1234);
    Value v_int32 = int32_t(42);
    Value v_int64 = int64_t(123456789);
    Value v_uint8 = uint8_t(100);
    Value v_uint16 = uint16_t(5000);
    Value v_uint32 = uint32_t(42);
    Value v_uint64 = uint64_t(987654321);
    Value v_float = 3.14f;
    Value v_double = 2.71828;
    Value v_string = std::string("hello");

    EXPECT_EQ(get_value_type(v_bool), ValueType::BOOL);
    EXPECT_EQ(get_value_type(v_int8), ValueType::INT8);
    EXPECT_EQ(get_value_type(v_int16), ValueType::INT16);
    EXPECT_EQ(get_value_type(v_int32), ValueType::INT32);
    EXPECT_EQ(get_value_type(v_int64), ValueType::INT64);
    EXPECT_EQ(get_value_type(v_uint8), ValueType::UINT8);
    EXPECT_EQ(get_value_type(v_uint16), ValueType::UINT16);
    EXPECT_EQ(get_value_type(v_uint32), ValueType::UINT32);
    EXPECT_EQ(get_value_type(v_uint64), ValueType::UINT64);
    EXPECT_EQ(get_value_type(v_float), ValueType::FLOAT);
    EXPECT_EQ(get_value_type(v_double), ValueType::DOUBLE);
    EXPECT_EQ(get_value_type(v_string), ValueType::STRING);

    // Verify we can extract values back
    EXPECT_EQ(std::get<bool>(v_bool), true);
    EXPECT_EQ(std::get<int8_t>(v_int8), 42);
    EXPECT_EQ(std::get<int16_t>(v_int16), 1234);
    EXPECT_EQ(std::get<int32_t>(v_int32), 42);
    EXPECT_EQ(std::get<int64_t>(v_int64), 123456789);
    EXPECT_EQ(std::get<uint8_t>(v_uint8), 100);
    EXPECT_EQ(std::get<uint16_t>(v_uint16), 5000);
    EXPECT_EQ(std::get<uint32_t>(v_uint32), 42u);
    EXPECT_EQ(std::get<uint64_t>(v_uint64), 987654321u);
    EXPECT_FLOAT_EQ(std::get<float>(v_float), 3.14f);
    EXPECT_DOUBLE_EQ(std::get<double>(v_double), 2.71828);
    EXPECT_EQ(std::get<std::string>(v_string), "hello");
}

TEST(ValueTest, ArrayTypes) {
    // Test all array types
    Value v_bool_array = std::vector<bool>{true, false, true};
    Value v_int8_array = std::vector<int8_t>{1, 2, 3};
    Value v_int16_array = std::vector<int16_t>{100, 200, 300};
    Value v_int32_array = std::vector<int32_t>{1, 2, 3};
    Value v_int64_array = std::vector<int64_t>{100, 200, 300};
    Value v_uint8_array = std::vector<uint8_t>{10, 20, 30};
    Value v_uint16_array = std::vector<uint16_t>{1000, 2000, 3000};
    Value v_uint32_array = std::vector<uint32_t>{10, 20, 30};
    Value v_uint64_array = std::vector<uint64_t>{1000, 2000, 3000};
    Value v_float_array = std::vector<float>{1.0f, 2.0f, 3.0f};
    Value v_double_array = std::vector<double>{1.1, 2.2, 3.3};
    Value v_string_array = std::vector<std::string>{"foo", "bar", "baz"};

    EXPECT_EQ(get_value_type(v_bool_array), ValueType::BOOL_ARRAY);
    EXPECT_EQ(get_value_type(v_int8_array), ValueType::INT8_ARRAY);
    EXPECT_EQ(get_value_type(v_int16_array), ValueType::INT16_ARRAY);
    EXPECT_EQ(get_value_type(v_int32_array), ValueType::INT32_ARRAY);
    EXPECT_EQ(get_value_type(v_int64_array), ValueType::INT64_ARRAY);
    EXPECT_EQ(get_value_type(v_uint8_array), ValueType::UINT8_ARRAY);
    EXPECT_EQ(get_value_type(v_uint16_array), ValueType::UINT16_ARRAY);
    EXPECT_EQ(get_value_type(v_uint32_array), ValueType::UINT32_ARRAY);
    EXPECT_EQ(get_value_type(v_uint64_array), ValueType::UINT64_ARRAY);
    EXPECT_EQ(get_value_type(v_float_array), ValueType::FLOAT_ARRAY);
    EXPECT_EQ(get_value_type(v_double_array), ValueType::DOUBLE_ARRAY);
    EXPECT_EQ(get_value_type(v_string_array), ValueType::STRING_ARRAY);

    // Verify array contents
    auto& bool_arr = std::get<std::vector<bool>>(v_bool_array);
    EXPECT_EQ(bool_arr.size(), 3);
    EXPECT_EQ(bool_arr[0], true);
    EXPECT_EQ(bool_arr[1], false);

    auto& int8_arr = std::get<std::vector<int8_t>>(v_int8_array);
    EXPECT_EQ(int8_arr.size(), 3);
    EXPECT_EQ(int8_arr[0], 1);
    EXPECT_EQ(int8_arr[2], 3);

    auto& uint8_arr = std::get<std::vector<uint8_t>>(v_uint8_array);
    EXPECT_EQ(uint8_arr.size(), 3);
    EXPECT_EQ(uint8_arr[0], 10);
    EXPECT_EQ(uint8_arr[2], 30);

    auto& string_arr = std::get<std::vector<std::string>>(v_string_array);
    EXPECT_EQ(string_arr.size(), 3);
    EXPECT_EQ(string_arr[0], "foo");
    EXPECT_EQ(string_arr[2], "baz");
}

TEST(ValueTest, TypeToString) {
    EXPECT_STREQ(value_type_to_string(ValueType::BOOL), "BOOL");
    EXPECT_STREQ(value_type_to_string(ValueType::INT8), "INT8");
    EXPECT_STREQ(value_type_to_string(ValueType::INT16), "INT16");
    EXPECT_STREQ(value_type_to_string(ValueType::UINT8), "UINT8");
    EXPECT_STREQ(value_type_to_string(ValueType::UINT16), "UINT16");
    EXPECT_STREQ(value_type_to_string(ValueType::FLOAT), "FLOAT");
    EXPECT_STREQ(value_type_to_string(ValueType::INT8_ARRAY), "INT8_ARRAY");
    EXPECT_STREQ(value_type_to_string(ValueType::UINT16_ARRAY), "UINT16_ARRAY");
    EXPECT_STREQ(value_type_to_string(ValueType::STRING_ARRAY), "STRING_ARRAY");
    EXPECT_STREQ(value_type_to_string(ValueType::STRUCT), "STRUCT");
}

TEST(ValueTest, TypeFromString) {
    EXPECT_EQ(value_type_from_string("bool"), ValueType::BOOL);
    EXPECT_EQ(value_type_from_string("INT8"), ValueType::INT8);
    EXPECT_EQ(value_type_from_string("int16"), ValueType::INT16);
    EXPECT_EQ(value_type_from_string("UINT8"), ValueType::UINT8);
    EXPECT_EQ(value_type_from_string("uint16"), ValueType::UINT16);
    EXPECT_EQ(value_type_from_string("FLOAT"), ValueType::FLOAT);
    EXPECT_EQ(value_type_from_string("int32"), ValueType::INT32);
    EXPECT_EQ(value_type_from_string("int8_array"), ValueType::INT8_ARRAY);
    EXPECT_EQ(value_type_from_string("INT8[]"), ValueType::INT8_ARRAY);
    EXPECT_EQ(value_type_from_string("uint16_array"), ValueType::UINT16_ARRAY);
    EXPECT_EQ(value_type_from_string("UINT16[]"), ValueType::UINT16_ARRAY);
    EXPECT_EQ(value_type_from_string("string_array"), ValueType::STRING_ARRAY);
    EXPECT_FALSE(value_type_from_string("invalid").has_value());
}

TEST(ValueTest, TypeCompatibility) {
    // Same type
    EXPECT_TRUE(are_types_compatible(ValueType::FLOAT, ValueType::FLOAT));

    // Float/double compatibility
    EXPECT_TRUE(are_types_compatible(ValueType::FLOAT, ValueType::DOUBLE));
    EXPECT_TRUE(are_types_compatible(ValueType::DOUBLE, ValueType::FLOAT));

    // Signed int compatibility (all signed integers are compatible)
    EXPECT_TRUE(are_types_compatible(ValueType::INT8, ValueType::INT16));
    EXPECT_TRUE(are_types_compatible(ValueType::INT8, ValueType::INT32));
    EXPECT_TRUE(are_types_compatible(ValueType::INT8, ValueType::INT64));
    EXPECT_TRUE(are_types_compatible(ValueType::INT16, ValueType::INT32));
    EXPECT_TRUE(are_types_compatible(ValueType::INT16, ValueType::INT64));
    EXPECT_TRUE(are_types_compatible(ValueType::INT32, ValueType::INT64));
    EXPECT_TRUE(are_types_compatible(ValueType::INT64, ValueType::INT32));

    // Unsigned int compatibility (all unsigned integers are compatible)
    EXPECT_TRUE(are_types_compatible(ValueType::UINT8, ValueType::UINT16));
    EXPECT_TRUE(are_types_compatible(ValueType::UINT8, ValueType::UINT32));
    EXPECT_TRUE(are_types_compatible(ValueType::UINT8, ValueType::UINT64));
    EXPECT_TRUE(are_types_compatible(ValueType::UINT16, ValueType::UINT32));
    EXPECT_TRUE(are_types_compatible(ValueType::UINT16, ValueType::UINT64));
    EXPECT_TRUE(are_types_compatible(ValueType::UINT32, ValueType::UINT64));
    EXPECT_TRUE(are_types_compatible(ValueType::UINT64, ValueType::UINT32));

    // Signed int array compatibility
    EXPECT_TRUE(are_types_compatible(ValueType::INT8_ARRAY, ValueType::INT16_ARRAY));
    EXPECT_TRUE(are_types_compatible(ValueType::INT8_ARRAY, ValueType::INT32_ARRAY));
    EXPECT_TRUE(are_types_compatible(ValueType::INT16_ARRAY, ValueType::INT64_ARRAY));

    // Unsigned int array compatibility
    EXPECT_TRUE(are_types_compatible(ValueType::UINT8_ARRAY, ValueType::UINT16_ARRAY));
    EXPECT_TRUE(are_types_compatible(ValueType::UINT8_ARRAY, ValueType::UINT32_ARRAY));
    EXPECT_TRUE(are_types_compatible(ValueType::UINT16_ARRAY, ValueType::UINT64_ARRAY));

    // Incompatible types
    EXPECT_FALSE(are_types_compatible(ValueType::INT8, ValueType::UINT8));
    EXPECT_FALSE(are_types_compatible(ValueType::INT32, ValueType::UINT32));
    EXPECT_FALSE(are_types_compatible(ValueType::FLOAT, ValueType::INT32));
    EXPECT_FALSE(are_types_compatible(ValueType::STRING, ValueType::BOOL));
    EXPECT_FALSE(are_types_compatible(ValueType::FLOAT, ValueType::FLOAT_ARRAY));
    EXPECT_FALSE(are_types_compatible(ValueType::INT8_ARRAY, ValueType::UINT8_ARRAY));
}

TEST(ValueTest, EmptyValue) {
    Value empty = std::monostate{};
    EXPECT_TRUE(is_empty(empty));
    EXPECT_EQ(get_value_type(empty), ValueType::UNSPECIFIED);

    Value not_empty = 42;
    EXPECT_FALSE(is_empty(not_empty));
}

TEST(ValueTest, TypeChecks) {
    EXPECT_TRUE(is_primitive(ValueType::FLOAT));
    EXPECT_TRUE(is_primitive(ValueType::STRING));
    EXPECT_FALSE(is_primitive(ValueType::FLOAT_ARRAY));
    EXPECT_FALSE(is_primitive(ValueType::STRUCT));

    EXPECT_TRUE(is_array(ValueType::FLOAT_ARRAY));
    EXPECT_TRUE(is_array(ValueType::STRUCT_ARRAY));
    EXPECT_FALSE(is_array(ValueType::FLOAT));
    EXPECT_FALSE(is_array(ValueType::STRUCT));

    EXPECT_TRUE(is_struct(ValueType::STRUCT));
    EXPECT_TRUE(is_struct(ValueType::STRUCT_ARRAY));
    EXPECT_FALSE(is_struct(ValueType::FLOAT));
}

// ============================================================================
// Value comparison tests
// ============================================================================

TEST(ValueComparisonTest, PrimitiveEquality) {
    // Same values
    EXPECT_TRUE(values_equal(Value{42}, Value{42}));
    EXPECT_TRUE(values_equal(Value{3.14}, Value{3.14}));
    EXPECT_TRUE(values_equal(Value{true}, Value{true}));
    EXPECT_TRUE(values_equal(Value{std::string("hello")}, Value{std::string("hello")}));

    // Different values, same type
    EXPECT_FALSE(values_equal(Value{42}, Value{43}));
    EXPECT_FALSE(values_equal(Value{3.14}, Value{2.71}));
    EXPECT_FALSE(values_equal(Value{true}, Value{false}));
    EXPECT_FALSE(values_equal(Value{std::string("hello")}, Value{std::string("world")}));

    // Different types
    EXPECT_FALSE(values_equal(Value{42}, Value{42.0}));
    EXPECT_FALSE(values_equal(Value{int32_t(42)}, Value{int64_t(42)}));
    EXPECT_FALSE(values_equal(Value{1}, Value{true}));
}

TEST(ValueComparisonTest, ArrayEquality) {
    // Same arrays
    EXPECT_TRUE(values_equal(
        Value{std::vector<int32_t>{1, 2, 3}},
        Value{std::vector<int32_t>{1, 2, 3}}));

    EXPECT_TRUE(values_equal(
        Value{std::vector<double>{1.1, 2.2, 3.3}},
        Value{std::vector<double>{1.1, 2.2, 3.3}}));

    EXPECT_TRUE(values_equal(
        Value{std::vector<std::string>{"a", "b", "c"}},
        Value{std::vector<std::string>{"a", "b", "c"}}));

    // Different arrays
    EXPECT_FALSE(values_equal(
        Value{std::vector<int32_t>{1, 2, 3}},
        Value{std::vector<int32_t>{1, 2, 4}}));

    EXPECT_FALSE(values_equal(
        Value{std::vector<int32_t>{1, 2, 3}},
        Value{std::vector<int32_t>{1, 2}}));

    // Different array types
    EXPECT_FALSE(values_equal(
        Value{std::vector<int32_t>{1, 2, 3}},
        Value{std::vector<int64_t>{1, 2, 3}}));
}

TEST(ValueComparisonTest, EmptyValues) {
    Value empty1 = std::monostate{};
    Value empty2 = std::monostate{};
    Value not_empty = 42;

    EXPECT_TRUE(values_equal(empty1, empty2));
    EXPECT_FALSE(values_equal(empty1, not_empty));
}

TEST(ValueComparisonTest, StructEquality) {
    // Create two identical structs
    auto struct1 = std::make_shared<StructValue>("TestType");
    struct1->set_field("x", Value{1.0});
    struct1->set_field("y", Value{2.0});

    auto struct2 = std::make_shared<StructValue>("TestType");
    struct2->set_field("x", Value{1.0});
    struct2->set_field("y", Value{2.0});

    EXPECT_TRUE(values_equal(Value{struct1}, Value{struct2}));

    // Different field values
    auto struct3 = std::make_shared<StructValue>("TestType");
    struct3->set_field("x", Value{1.0});
    struct3->set_field("y", Value{3.0});  // Different!

    EXPECT_FALSE(values_equal(Value{struct1}, Value{struct3}));

    // Different type name
    auto struct4 = std::make_shared<StructValue>("OtherType");
    struct4->set_field("x", Value{1.0});
    struct4->set_field("y", Value{2.0});

    EXPECT_FALSE(values_equal(Value{struct1}, Value{struct4}));

    // Different fields
    auto struct5 = std::make_shared<StructValue>("TestType");
    struct5->set_field("x", Value{1.0});
    struct5->set_field("z", Value{2.0});  // Different field name

    EXPECT_FALSE(values_equal(Value{struct1}, Value{struct5}));
}

TEST(ValueComparisonTest, NestedStructEquality) {
    // Create nested structs
    auto inner1 = std::make_shared<StructValue>("Inner");
    inner1->set_field("value", Value{42});

    auto outer1 = std::make_shared<StructValue>("Outer");
    outer1->set_field("nested", Value{inner1});
    outer1->set_field("name", Value{std::string("test")});

    auto inner2 = std::make_shared<StructValue>("Inner");
    inner2->set_field("value", Value{42});

    auto outer2 = std::make_shared<StructValue>("Outer");
    outer2->set_field("nested", Value{inner2});
    outer2->set_field("name", Value{std::string("test")});

    EXPECT_TRUE(values_equal(Value{outer1}, Value{outer2}));

    // Change nested value
    auto inner3 = std::make_shared<StructValue>("Inner");
    inner3->set_field("value", Value{99});  // Different!

    auto outer3 = std::make_shared<StructValue>("Outer");
    outer3->set_field("nested", Value{inner3});
    outer3->set_field("name", Value{std::string("test")});

    EXPECT_FALSE(values_equal(Value{outer1}, Value{outer3}));
}

// ============================================================================
// to_double tests
// ============================================================================

TEST(ToDoubleTest, NumericTypes) {
    EXPECT_DOUBLE_EQ(to_double(Value{int8_t(42)}), 42.0);
    EXPECT_DOUBLE_EQ(to_double(Value{int16_t(-100)}), -100.0);
    EXPECT_DOUBLE_EQ(to_double(Value{int32_t(12345)}), 12345.0);
    EXPECT_DOUBLE_EQ(to_double(Value{int64_t(9876543210LL)}), 9876543210.0);

    EXPECT_DOUBLE_EQ(to_double(Value{uint8_t(255)}), 255.0);
    EXPECT_DOUBLE_EQ(to_double(Value{uint16_t(65535)}), 65535.0);
    EXPECT_DOUBLE_EQ(to_double(Value{uint32_t(123456)}), 123456.0);
    EXPECT_DOUBLE_EQ(to_double(Value{uint64_t(9876543210ULL)}), 9876543210.0);

    EXPECT_FLOAT_EQ(to_double(Value{3.14f}), 3.14);
    EXPECT_DOUBLE_EQ(to_double(Value{2.71828}), 2.71828);
}

TEST(ToDoubleTest, BoolType) {
    EXPECT_DOUBLE_EQ(to_double(Value{true}), 1.0);
    EXPECT_DOUBLE_EQ(to_double(Value{false}), 0.0);
}

TEST(ToDoubleTest, NonNumericTypes) {
    EXPECT_DOUBLE_EQ(to_double(Value{std::string("hello")}), 0.0);
    EXPECT_DOUBLE_EQ(to_double(Value{std::monostate{}}), 0.0);
    EXPECT_DOUBLE_EQ(to_double(Value{std::vector<int32_t>{1, 2, 3}}), 0.0);

    auto s = std::make_shared<StructValue>("Test");
    EXPECT_DOUBLE_EQ(to_double(Value{s}), 0.0);
}

// ============================================================================
// value_changed_beyond_threshold tests
// ============================================================================

TEST(ThresholdTest, NumericWithThreshold) {
    Value v1 = 100.0;
    Value v2 = 100.5;
    Value v3 = 105.0;

    // Change of 0.5 is below threshold of 1.0
    EXPECT_FALSE(value_changed_beyond_threshold(v1, v2, 1.0));

    // Change of 5.0 exceeds threshold of 1.0
    EXPECT_TRUE(value_changed_beyond_threshold(v1, v3, 1.0));

    // Exact threshold boundary
    Value v4 = 101.0;
    EXPECT_TRUE(value_changed_beyond_threshold(v1, v4, 1.0));  // >= threshold
}

TEST(ThresholdTest, IntegerWithThreshold) {
    Value v1 = int32_t(100);
    Value v2 = int32_t(102);
    Value v3 = int32_t(110);

    EXPECT_FALSE(value_changed_beyond_threshold(v1, v2, 5.0));
    EXPECT_TRUE(value_changed_beyond_threshold(v1, v3, 5.0));
}

TEST(ThresholdTest, ZeroThreshold) {
    Value v1 = 100.0;
    Value v2 = 100.0;
    Value v3 = 100.001;

    // With zero threshold, any difference is a change
    EXPECT_FALSE(value_changed_beyond_threshold(v1, v2, 0.0));  // Same value
    EXPECT_TRUE(value_changed_beyond_threshold(v1, v3, 0.0));   // Tiny difference
}

TEST(ThresholdTest, NonNumericAlwaysCompareExact) {
    Value s1 = std::string("hello");
    Value s2 = std::string("hello");
    Value s3 = std::string("world");

    // Threshold is ignored for non-numeric types
    EXPECT_FALSE(value_changed_beyond_threshold(s1, s2, 1000.0));
    EXPECT_TRUE(value_changed_beyond_threshold(s1, s3, 1000.0));
}

TEST(ThresholdTest, DifferentTypes) {
    Value v1 = 100.0;
    Value v2 = int32_t(100);

    // Different types always considered a change
    EXPECT_TRUE(value_changed_beyond_threshold(v1, v2, 1000.0));
}

TEST(ThresholdTest, StructComparison) {
    auto struct1 = std::make_shared<StructValue>("Test");
    struct1->set_field("speed", Value{50.0});

    auto struct2 = std::make_shared<StructValue>("Test");
    struct2->set_field("speed", Value{50.0});

    auto struct3 = std::make_shared<StructValue>("Test");
    struct3->set_field("speed", Value{60.0});

    // Structs compared by deep equality (threshold ignored)
    EXPECT_FALSE(value_changed_beyond_threshold(Value{struct1}, Value{struct2}, 100.0));
    EXPECT_TRUE(value_changed_beyond_threshold(Value{struct1}, Value{struct3}, 100.0));
}
