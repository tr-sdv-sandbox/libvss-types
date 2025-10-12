/**
 * @file test_value.cpp
 * @brief Basic tests for value types
 */

#include <vss/types/value.hpp>
#include <gtest/gtest.h>

using namespace vss::types;

TEST(ValueTest, PrimitiveTypes) {
    // Test all primitive types
    Value v_bool = true;
    Value v_int32 = int32_t(42);
    Value v_uint32 = uint32_t(42);
    Value v_int64 = int64_t(123456789);
    Value v_uint64 = uint64_t(987654321);
    Value v_float = 3.14f;
    Value v_double = 2.71828;
    Value v_string = std::string("hello");

    EXPECT_EQ(get_value_type(v_bool), ValueType::BOOL);
    EXPECT_EQ(get_value_type(v_int32), ValueType::INT32);
    EXPECT_EQ(get_value_type(v_uint32), ValueType::UINT32);
    EXPECT_EQ(get_value_type(v_int64), ValueType::INT64);
    EXPECT_EQ(get_value_type(v_uint64), ValueType::UINT64);
    EXPECT_EQ(get_value_type(v_float), ValueType::FLOAT);
    EXPECT_EQ(get_value_type(v_double), ValueType::DOUBLE);
    EXPECT_EQ(get_value_type(v_string), ValueType::STRING);

    // Verify we can extract values back
    EXPECT_EQ(std::get<bool>(v_bool), true);
    EXPECT_EQ(std::get<int32_t>(v_int32), 42);
    EXPECT_EQ(std::get<uint32_t>(v_uint32), 42u);
    EXPECT_EQ(std::get<int64_t>(v_int64), 123456789);
    EXPECT_EQ(std::get<uint64_t>(v_uint64), 987654321u);
    EXPECT_FLOAT_EQ(std::get<float>(v_float), 3.14f);
    EXPECT_DOUBLE_EQ(std::get<double>(v_double), 2.71828);
    EXPECT_EQ(std::get<std::string>(v_string), "hello");
}

TEST(ValueTest, ArrayTypes) {
    // Test all array types
    Value v_bool_array = std::vector<bool>{true, false, true};
    Value v_int32_array = std::vector<int32_t>{1, 2, 3};
    Value v_uint32_array = std::vector<uint32_t>{10, 20, 30};
    Value v_int64_array = std::vector<int64_t>{100, 200, 300};
    Value v_uint64_array = std::vector<uint64_t>{1000, 2000, 3000};
    Value v_float_array = std::vector<float>{1.0f, 2.0f, 3.0f};
    Value v_double_array = std::vector<double>{1.1, 2.2, 3.3};
    Value v_string_array = std::vector<std::string>{"foo", "bar", "baz"};

    EXPECT_EQ(get_value_type(v_bool_array), ValueType::BOOL_ARRAY);
    EXPECT_EQ(get_value_type(v_int32_array), ValueType::INT32_ARRAY);
    EXPECT_EQ(get_value_type(v_uint32_array), ValueType::UINT32_ARRAY);
    EXPECT_EQ(get_value_type(v_int64_array), ValueType::INT64_ARRAY);
    EXPECT_EQ(get_value_type(v_uint64_array), ValueType::UINT64_ARRAY);
    EXPECT_EQ(get_value_type(v_float_array), ValueType::FLOAT_ARRAY);
    EXPECT_EQ(get_value_type(v_double_array), ValueType::DOUBLE_ARRAY);
    EXPECT_EQ(get_value_type(v_string_array), ValueType::STRING_ARRAY);

    // Verify array contents
    auto& bool_arr = std::get<std::vector<bool>>(v_bool_array);
    EXPECT_EQ(bool_arr.size(), 3);
    EXPECT_EQ(bool_arr[0], true);
    EXPECT_EQ(bool_arr[1], false);

    auto& int32_arr = std::get<std::vector<int32_t>>(v_int32_array);
    EXPECT_EQ(int32_arr.size(), 3);
    EXPECT_EQ(int32_arr[0], 1);
    EXPECT_EQ(int32_arr[2], 3);

    auto& string_arr = std::get<std::vector<std::string>>(v_string_array);
    EXPECT_EQ(string_arr.size(), 3);
    EXPECT_EQ(string_arr[0], "foo");
    EXPECT_EQ(string_arr[2], "baz");
}

TEST(ValueTest, TypeToString) {
    EXPECT_STREQ(value_type_to_string(ValueType::BOOL), "BOOL");
    EXPECT_STREQ(value_type_to_string(ValueType::FLOAT), "FLOAT");
    EXPECT_STREQ(value_type_to_string(ValueType::STRING_ARRAY), "STRING_ARRAY");
    EXPECT_STREQ(value_type_to_string(ValueType::STRUCT), "STRUCT");
}

TEST(ValueTest, TypeFromString) {
    EXPECT_EQ(value_type_from_string("bool"), ValueType::BOOL);
    EXPECT_EQ(value_type_from_string("FLOAT"), ValueType::FLOAT);
    EXPECT_EQ(value_type_from_string("int32"), ValueType::INT32);
    EXPECT_EQ(value_type_from_string("string_array"), ValueType::STRING_ARRAY);
    EXPECT_FALSE(value_type_from_string("invalid").has_value());
}

TEST(ValueTest, TypeCompatibility) {
    // Same type
    EXPECT_TRUE(are_types_compatible(ValueType::FLOAT, ValueType::FLOAT));

    // Float/double compatibility
    EXPECT_TRUE(are_types_compatible(ValueType::FLOAT, ValueType::DOUBLE));
    EXPECT_TRUE(are_types_compatible(ValueType::DOUBLE, ValueType::FLOAT));

    // Signed int compatibility
    EXPECT_TRUE(are_types_compatible(ValueType::INT32, ValueType::INT64));
    EXPECT_TRUE(are_types_compatible(ValueType::INT64, ValueType::INT32));

    // Unsigned int compatibility
    EXPECT_TRUE(are_types_compatible(ValueType::UINT32, ValueType::UINT64));
    EXPECT_TRUE(are_types_compatible(ValueType::UINT64, ValueType::UINT32));

    // Incompatible types
    EXPECT_FALSE(are_types_compatible(ValueType::INT32, ValueType::UINT32));
    EXPECT_FALSE(are_types_compatible(ValueType::FLOAT, ValueType::INT32));
    EXPECT_FALSE(are_types_compatible(ValueType::STRING, ValueType::BOOL));
    EXPECT_FALSE(are_types_compatible(ValueType::FLOAT, ValueType::FLOAT_ARRAY));
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
