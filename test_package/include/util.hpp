// Utilities for testing
//
// Author: Matthew Knight
// File Name: util.hpp
// Date: 2019-10-16

#pragma once

#include <vector>
#include <stdexcept>
#include <iterator>
#include <string>

#include <cstdint>
#include <cstring>

std::vector<std::uint8_t> operator""_hex(char const* hex, std::size_t size) {
    // must be even number of digits
    if (size % 2 != 0)
        throw std::invalid_argument("need even number of digits");

    std::vector<std::uint8_t> ret;
    auto const begin = hex;
    auto const end = hex + size;
    char buf[3] = {0, 0, 0};

    for (auto it = begin; it != end; it += 2) {
        std::copy(it, std::next(it, 2), std::begin(buf));
        ret.push_back(std::strtoul(buf, nullptr, 16));
    }

    return ret;
}
