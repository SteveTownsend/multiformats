// Unit tests for Varint
//
// Author: Matthew Knight
// File Name: varint-test.cpp
// Date: 2019-10-15

#include "multiformats/varint.hpp"

#include <gtest/gtest.h>

#include <vector>

struct VarintTestParameter {
    std::uint64_t value;
    std::vector<std::uint8_t> buf;
};

std::vector<VarintTestParameter> const parameters{{0, {0x00}},
                                                  {1, {0x01}},
                                                  {127, {0x7f}},
                                                  {128, {0x80, 0x01}},
                                                  {300, {0xac, 0x02}}};

class VarintParamTestFixture
    : public ::testing::TestWithParam<VarintTestParameter> {};

TEST_P(VarintParamTestFixture, MakeVarint) {
    auto [value, buf] = GetParam();
    auto [varint, it] = make_varint(buf.begin(), buf.end());

    EXPECT_EQ(varint, value);
    EXPECT_TRUE(std::equal(varint.begin(), varint.end(), buf.cbegin()));
}

TEST_P(VarintParamTestFixture, ValueToBuf) {
    auto [value, buf] = GetParam();
    Varint varint{value};

    EXPECT_EQ(varint.size(), buf.size());
    EXPECT_TRUE(std::equal(varint.begin(), varint.end(), buf.cbegin()));
}

TEST_P(VarintParamTestFixture, BufToValue) {
    auto [value, buf] = GetParam();
    Varint varint{buf.cbegin(), buf.cend()};

    EXPECT_EQ(varint, value);
}

INSTANTIATE_TEST_CASE_P(VarintTests, VarintParamTestFixture,
                        ::testing::ValuesIn(parameters));
