// Unsigned variable length integer
//
// Author: Matthew Knight
// File Name: varint.hpp
// Date: 2019-09-12

#pragma once

#include <algorithm>
#include <type_traits>
#include <vector>

class Varint {
    static constexpr auto max_bits = 63;
    static constexpr unsigned long long max = 0xefffffffffffffff;

    std::vector<std::uint8_t> buf{};

  public:
    template <typename Integral,
              typename = std::enable_if_t<std::is_signed_v<Integral> &&
                                          std::is_integral_v<Integral>>>
    Varint(Integral integral) {
        if constexpr (std::numeric_limits<Integral>::digits > max_bits)
            if (integral > max)
                throw std::runtime_error(
                    "value is too large to store in a varint");

        do {
            buf.emplace(integral & 0xef);
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
