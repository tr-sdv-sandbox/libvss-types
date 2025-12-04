/**
 * @file test_quality.cpp
 * @brief Tests for signal quality types
 */

#include <vss/types/quality.hpp>
#include <vss/types/struct.hpp>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

using namespace vss::types;
using namespace std::chrono_literals;

TEST(QualityTest, SignalQualityToString) {
    EXPECT_STREQ(signal_quality_to_string(SignalQuality::VALID), "VALID");
    EXPECT_STREQ(signal_quality_to_string(SignalQuality::INVALID), "INVALID");
    EXPECT_STREQ(signal_quality_to_string(SignalQuality::NOT_AVAILABLE), "NOT_AVAILABLE");
}

TEST(QualityTest, SignalQualityFromString) {
    EXPECT_EQ(signal_quality_from_string("valid"), SignalQuality::VALID);
    EXPECT_EQ(signal_quality_from_string("INVALID"), SignalQuality::INVALID);
    EXPECT_EQ(signal_quality_from_string("not_available"), SignalQuality::NOT_AVAILABLE);
    EXPECT_FALSE(signal_quality_from_string("unknown_quality").has_value());
}

TEST(QualityTest, QualifiedValueBasic) {
    QualifiedValue<float> temp{22.5f};

    EXPECT_TRUE(temp.value.has_value());
    EXPECT_EQ(*temp.value, 22.5f);
    EXPECT_EQ(temp.quality, SignalQuality::VALID);
    EXPECT_TRUE(temp.is_valid());
    EXPECT_FALSE(temp.is_invalid());
}

TEST(QualityTest, QualifiedValueWithQuality) {
    QualifiedValue<float> invalid{100.0f, SignalQuality::INVALID};

    EXPECT_TRUE(invalid.value.has_value());
    EXPECT_EQ(invalid.quality, SignalQuality::INVALID);
    EXPECT_FALSE(invalid.is_valid());
    EXPECT_TRUE(invalid.is_invalid());
}

TEST(QualityTest, QualifiedValueNotAvailable) {
    QualifiedValue<float> unavailable;
    unavailable.quality = SignalQuality::NOT_AVAILABLE;

    EXPECT_FALSE(unavailable.value.has_value());
    EXPECT_TRUE(unavailable.is_not_available());
    EXPECT_FALSE(unavailable.is_valid());
}


TEST(QualityTest, QualifiedValueValueOr) {
    QualifiedValue<int> with_value{42};
    EXPECT_EQ(with_value.value_or(99), 42);

    QualifiedValue<int> without_value;
    EXPECT_EQ(without_value.value_or(99), 99);
}

TEST(QualityTest, QualifiedValueAge) {
    QualifiedValue<float> temp{22.5f};

    // Age should be very small initially
    auto age1 = temp.age();
    EXPECT_LT(age1.count(), 100);  // Less than 100ms

    // Wait a bit
    std::this_thread::sleep_for(50ms);

    // Age should have increased
    auto age2 = temp.age();
    EXPECT_GT(age2.count(), age1.count());
    EXPECT_GE(age2.count(), 50);  // At least 50ms
}

TEST(QualityTest, DynamicQualifiedValue) {
    DynamicQualifiedValue dqv{Value{42}};

    EXPECT_FALSE(is_empty(dqv.value));
    EXPECT_EQ(dqv.quality, SignalQuality::VALID);
    EXPECT_TRUE(dqv.is_valid());

    EXPECT_EQ(std::get<int>(dqv.value), 42);
}

TEST(QualityTest, DynamicQualifiedValueInvalid) {
    DynamicQualifiedValue dqv{Value{100.0f}, SignalQuality::INVALID};

    EXPECT_TRUE(dqv.is_invalid());
    EXPECT_FALSE(dqv.is_valid());
}

// =============================================================================
// QualifiedValue Comparison Tests
// =============================================================================

TEST(QualifiedValueComparisonTest, EqualityOperator) {
    QualifiedValue<float> a{42.5f};
    QualifiedValue<float> b{42.5f};
    QualifiedValue<float> c{99.0f};

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
}

TEST(QualifiedValueComparisonTest, EqualityIgnoresTimestamp) {
    QualifiedValue<int> a{100};
    std::this_thread::sleep_for(10ms);
    QualifiedValue<int> b{100};

    // Different timestamps but same value and quality
    EXPECT_TRUE(a == b);
}

TEST(QualifiedValueComparisonTest, QualityAffectsEquality) {
    QualifiedValue<float> valid{42.5f, SignalQuality::VALID};
    QualifiedValue<float> invalid{42.5f, SignalQuality::INVALID};

    // Same value but different quality
    EXPECT_FALSE(valid == invalid);
}

TEST(QualifiedValueComparisonTest, EmptyValuesEqual) {
    QualifiedValue<float> a;
    QualifiedValue<float> b;

    EXPECT_TRUE(a == b);
}

TEST(QualifiedValueComparisonTest, ThresholdNumeric) {
    QualifiedValue<float> a{100.0f};
    QualifiedValue<float> b{100.5f};
    QualifiedValue<float> c{105.0f};

    // 0.5 change with threshold 1.0 - not significant
    EXPECT_FALSE(qualified_value_changed_beyond_threshold(a, b, 1.0));

    // 5.0 change with threshold 1.0 - significant
    EXPECT_TRUE(qualified_value_changed_beyond_threshold(a, c, 1.0));

    // 0.5 change with threshold 0 - any change counts
    EXPECT_TRUE(qualified_value_changed_beyond_threshold(a, b, 0.0));
}

TEST(QualifiedValueComparisonTest, ThresholdInteger) {
    QualifiedValue<int32_t> a{100};
    QualifiedValue<int32_t> b{102};
    QualifiedValue<int32_t> c{110};

    // 2 change with threshold 5 - not significant
    EXPECT_FALSE(qualified_value_changed_beyond_threshold(a, b, 5.0));

    // 10 change with threshold 5 - significant
    EXPECT_TRUE(qualified_value_changed_beyond_threshold(a, c, 5.0));
}

TEST(QualifiedValueComparisonTest, ThresholdQualityChange) {
    QualifiedValue<float> valid{100.0f, SignalQuality::VALID};
    QualifiedValue<float> invalid{100.0f, SignalQuality::INVALID};

    // Same value but quality changed - always significant
    EXPECT_TRUE(qualified_value_changed_beyond_threshold(valid, invalid, 100.0));
}

TEST(QualifiedValueComparisonTest, ThresholdString) {
    QualifiedValue<std::string> a{"hello"};
    QualifiedValue<std::string> b{"hello"};
    QualifiedValue<std::string> c{"world"};

    // Same string - no change
    EXPECT_FALSE(qualified_value_changed_beyond_threshold(a, b, 1.0));

    // Different string - always change (threshold ignored for strings)
    EXPECT_TRUE(qualified_value_changed_beyond_threshold(a, c, 1.0));
}

TEST(QualifiedValueComparisonTest, ThresholdEmptyToValue) {
    QualifiedValue<float> empty;
    QualifiedValue<float> with_value{100.0f};

    // Empty to value - always a change
    EXPECT_TRUE(qualified_value_changed_beyond_threshold(empty, with_value, 100.0));
}

// =============================================================================
// DynamicQualifiedValue Comparison Tests
// =============================================================================

TEST(DynamicQualifiedValueComparisonTest, EqualityOperator) {
    DynamicQualifiedValue a{Value{42.5f}};
    DynamicQualifiedValue b{Value{42.5f}};
    DynamicQualifiedValue c{Value{99.0f}};

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}

TEST(DynamicQualifiedValueComparisonTest, EqualityWithStruct) {
    auto struct1 = std::make_shared<StructValue>("Position");
    struct1->set_field("x", 1.0);
    struct1->set_field("y", 2.0);

    auto struct2 = std::make_shared<StructValue>("Position");
    struct2->set_field("x", 1.0);
    struct2->set_field("y", 2.0);

    auto struct3 = std::make_shared<StructValue>("Position");
    struct3->set_field("x", 1.0);
    struct3->set_field("y", 3.0);  // Different y

    DynamicQualifiedValue a{Value{struct1}};
    DynamicQualifiedValue b{Value{struct2}};
    DynamicQualifiedValue c{Value{struct3}};

    EXPECT_TRUE(a == b);  // Deep equality
    EXPECT_FALSE(a == c);
}

TEST(DynamicQualifiedValueComparisonTest, FunctionEquality) {
    DynamicQualifiedValue a{Value{42}};
    DynamicQualifiedValue b{Value{42}};

    EXPECT_TRUE(dynamic_qualified_values_equal(a, b));
}

TEST(DynamicQualifiedValueComparisonTest, ThresholdNumeric) {
    DynamicQualifiedValue a{Value{100.0}};
    DynamicQualifiedValue b{Value{100.5}};
    DynamicQualifiedValue c{Value{110.0}};

    // 0.5 change with threshold 1.0 - not significant
    EXPECT_FALSE(dynamic_qualified_value_changed_beyond_threshold(a, b, 1.0));

    // 10.0 change with threshold 1.0 - significant
    EXPECT_TRUE(dynamic_qualified_value_changed_beyond_threshold(a, c, 1.0));
}

TEST(DynamicQualifiedValueComparisonTest, ThresholdQualityChange) {
    DynamicQualifiedValue valid{Value{100.0}, SignalQuality::VALID};
    DynamicQualifiedValue invalid{Value{100.0}, SignalQuality::INVALID};

    // Quality change always significant
    EXPECT_TRUE(dynamic_qualified_value_changed_beyond_threshold(valid, invalid, 1000.0));
}

TEST(DynamicQualifiedValueComparisonTest, ThresholdStruct) {
    auto struct1 = std::make_shared<StructValue>("Pos");
    struct1->set_field("x", 1.0);

    auto struct2 = std::make_shared<StructValue>("Pos");
    struct2->set_field("x", 1.0);

    auto struct3 = std::make_shared<StructValue>("Pos");
    struct3->set_field("x", 2.0);

    DynamicQualifiedValue a{Value{struct1}};
    DynamicQualifiedValue b{Value{struct2}};
    DynamicQualifiedValue c{Value{struct3}};

    // Same struct - no change
    EXPECT_FALSE(dynamic_qualified_value_changed_beyond_threshold(a, b, 0.0));

    // Different struct - change (deep comparison)
    EXPECT_TRUE(dynamic_qualified_value_changed_beyond_threshold(a, c, 0.0));
}
