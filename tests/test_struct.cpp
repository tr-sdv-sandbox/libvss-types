/**
 * @file test_struct.cpp
 * @brief Tests for struct support
 */

#include <vss/types/struct.hpp>
#include <gtest/gtest.h>

using namespace vss::types;

TEST(StructTest, StructDefinitionBasic) {
    StructDefinition def("DeliveryInfo", "Delivery information");

    EXPECT_EQ(def.type_name(), "DeliveryInfo");
    EXPECT_EQ(def.description(), "Delivery information");
    EXPECT_TRUE(def.fields().empty());
}

TEST(StructTest, StructDefinitionWithFields) {
    StructDefinition def("DeliveryInfo");
    def.add_field(FieldDefinition("Address", ValueType::STRING, "Destination address"))
       .add_field(FieldDefinition("Receiver", ValueType::STRING, "Name of receiver"));

    EXPECT_EQ(def.fields().size(), 2);
    EXPECT_TRUE(def.has_field("Address"));
    EXPECT_TRUE(def.has_field("Receiver"));
    EXPECT_FALSE(def.has_field("Unknown"));

    const auto* addr_field = def.get_field("Address");
    ASSERT_NE(addr_field, nullptr);
    EXPECT_EQ(addr_field->name, "Address");
    EXPECT_EQ(addr_field->type, ValueType::STRING);
}

TEST(StructTest, StructValueBasic) {
    StructValue value("DeliveryInfo");

    EXPECT_EQ(value.type_name(), "DeliveryInfo");
    EXPECT_TRUE(value.fields().empty());
}

TEST(StructTest, StructValueSetGet) {
    StructValue value("DeliveryInfo");
    value.set_field("Address", std::string("123 Main St"));
    value.set_field("Receiver", std::string("John Doe"));

    EXPECT_TRUE(value.has_field("Address"));
    EXPECT_TRUE(value.has_field("Receiver"));
    EXPECT_FALSE(value.has_field("Unknown"));

    const auto* addr = value.get_field("Address");
    ASSERT_NE(addr, nullptr);
    EXPECT_EQ(std::get<std::string>(*addr), "123 Main St");

    const auto* receiver = value.get_field("Receiver");
    ASSERT_NE(receiver, nullptr);
    EXPECT_EQ(std::get<std::string>(*receiver), "John Doe");
}

TEST(StructTest, StructValueRemove) {
    StructValue value("DeliveryInfo");
    value.set_field("Address", std::string("123 Main St"));
    value.set_field("Receiver", std::string("John Doe"));

    EXPECT_TRUE(value.remove_field("Address"));
    EXPECT_FALSE(value.has_field("Address"));
    EXPECT_TRUE(value.has_field("Receiver"));

    EXPECT_FALSE(value.remove_field("Unknown"));
}

TEST(StructTest, StructValueClear) {
    StructValue value("DeliveryInfo");
    value.set_field("Address", std::string("123 Main St"));
    value.set_field("Receiver", std::string("John Doe"));

    value.clear();
    EXPECT_TRUE(value.fields().empty());
}

TEST(StructTest, StructRegistry) {
    StructRegistry registry;

    StructDefinition def1("DeliveryInfo");
    def1.add_field(FieldDefinition("Address", ValueType::STRING));

    StructDefinition def2("Position");
    def2.add_field(FieldDefinition("Latitude", ValueType::DOUBLE))
        .add_field(FieldDefinition("Longitude", ValueType::DOUBLE));

    EXPECT_TRUE(registry.register_struct(std::move(def1)));
    EXPECT_TRUE(registry.register_struct(std::move(def2)));

    // Cannot register same type twice
    StructDefinition def1_duplicate("DeliveryInfo");
    EXPECT_FALSE(registry.register_struct(std::move(def1_duplicate)));

    EXPECT_TRUE(registry.has_struct("DeliveryInfo"));
    EXPECT_TRUE(registry.has_struct("Position"));
    EXPECT_FALSE(registry.has_struct("Unknown"));

    const auto* delivery_def = registry.get_struct("DeliveryInfo");
    ASSERT_NE(delivery_def, nullptr);
    EXPECT_EQ(delivery_def->type_name(), "DeliveryInfo");
}

TEST(StructTest, ValidateStruct) {
    StructRegistry registry;

    StructDefinition def("DeliveryInfo");
    def.add_field(FieldDefinition("Address", ValueType::STRING))
       .add_field(FieldDefinition("Receiver", ValueType::STRING));
    registry.register_struct(std::move(def));

    // Valid struct
    StructValue valid("DeliveryInfo");
    valid.set_field("Address", std::string("123 Main St"));
    valid.set_field("Receiver", std::string("John Doe"));
    EXPECT_FALSE(validate_struct(valid, registry).has_value());

    // Missing field
    StructValue missing("DeliveryInfo");
    missing.set_field("Address", std::string("123 Main St"));
    auto error = validate_struct(missing, registry);
    EXPECT_TRUE(error.has_value());
    EXPECT_NE(error->find("Receiver"), std::string::npos);

    // Wrong type
    StructValue wrong_type("DeliveryInfo");
    wrong_type.set_field("Address", std::string("123 Main St"));
    wrong_type.set_field("Receiver", 42);  // Should be string
    error = validate_struct(wrong_type, registry);
    EXPECT_TRUE(error.has_value());

    // Unknown struct type
    StructValue unknown("UnknownType");
    error = validate_struct(unknown, registry);
    EXPECT_TRUE(error.has_value());
    EXPECT_NE(error->find("not found"), std::string::npos);
}

TEST(StructTest, CreateDefaultStruct) {
    StructRegistry registry;

    StructDefinition def("TestStruct");
    FieldDefinition field1("Field1", ValueType::STRING);
    field1.default_value = std::string("default");
    def.add_field(field1);

    FieldDefinition field2("Field2", ValueType::INT32);
    field2.default_value = int32_t(42);
    def.add_field(field2);

    registry.register_struct(std::move(def));

    auto value = create_default_struct("TestStruct", registry);
    ASSERT_TRUE(value.has_value());

    EXPECT_TRUE(value->has_field("Field1"));
    EXPECT_TRUE(value->has_field("Field2"));

    const auto* f1 = value->get_field("Field1");
    ASSERT_NE(f1, nullptr);
    EXPECT_EQ(std::get<std::string>(*f1), "default");

    const auto* f2 = value->get_field("Field2");
    ASSERT_NE(f2, nullptr);
    EXPECT_EQ(std::get<int32_t>(*f2), 42);
}
