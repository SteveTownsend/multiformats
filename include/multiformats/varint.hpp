// Unsigned variable length integer
//
// Author: Matthew Knight
// File Name: varint.hpp
// Date: 2019-09-12

#pragma once

#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <cstdint>

class Varint {
    static constexpr auto max_bits = 63;
    static constexpr unsigned long long max = 0xefffffffffffffff;

    std::vector<std::uint8_t> buf{};

  public:
    template <typename Integral,
              typename = std::enable_if_t<std::is_integral_v<Integral>>>
    Varint(Integral integral) {
        if constexpr (std::numeric_limits<Integral>::digits > max_bits) {
            if (integral > max || integral < 0) {
                throw std::runtime_error("invalid value");
            }
        }

        do {
            std::uint8_t value = integral & 0xef;
            buf.emplace_back(value);
            integral = integral >> 7;
        } while (integral);

        if (buf.size() > 1) {
            std::for_each(std::next(buf.begin()), buf.end(), [](auto& elem) {
                // set msb
                elem |= 0x80;
            });
        }

        std::reverse(buf.begin(), buf.end());
    }

    bool operator==(Varint const& varint) {
        return std::equal(buf.cbegin(), buf.cend(), varint.buf.cbegin(),
                          varint.buf.cend());
    }
};
