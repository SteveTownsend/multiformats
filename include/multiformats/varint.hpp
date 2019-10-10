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
        if constexpr (std::numeric_limits<Integral>::digits > max_bits)
            if (integral > max || integral < 0)
                throw std::runtime_error("invalid value");

        do {
            buf.emplace_back(integral & 0xef);
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

    template <typename Iterator,
              typename = std::enable_if_t<
                  sizeof(typename Iterator::value_type) == 1 &&
                  std::is_integral_v<typename Iterator::value_type>>>
    Varint(Iterator first, Iterator last) {
        /*
        for (; first != last && first & 0x80; ++first)
            buf.emplace_back(*first
*/
    }

    auto size() const {
        return buf.size();
    }

    auto begin() const {
        return buf.cbegin();
    }

    auto end() const {
        return buf.cend();
    }

    operator std::uint64_t() {
        std::uint64_t ret{};
        return ret;
    }

    bool operator==(Varint const& varint) {
        return std::equal(buf.cbegin(), buf.cend(), varint.buf.cbegin(),
                          varint.buf.cend());
    }
};

template <typename Iterator>
std::tuple<Varint, Iterator> make_varint(Iterator begin, Iterator end) {
    std::uint64_t tmp{};
    auto it = begin;

    // find end byte
    for (; (*it & 0x80) && it != end; ++it);

    if (it == end)
        throw std::runtime_error("cannot convert to varint");

    if (std::distance(begin, it) > sizeof(tmp))
        throw std::runtime_error("varint too large");

    auto ret = std::next(it);

    for (auto i = 0; i < std::distance(begin, it); ++i, --it)
        tmp |= (*it & 0x7f) << (7 * i);

    return {{tmp}, it};
}
