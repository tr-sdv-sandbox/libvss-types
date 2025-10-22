/**
 * @file test_quality.cpp
 * @brief Tests for signal quality types
 */

#include <vss/types/quality.hpp>
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
