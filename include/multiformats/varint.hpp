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
            std::uint8_t value = integral & 0x7f;
            integral = integral >> 7;

            if (integral)
                value |= 0x80;

            buf.push_back(value);
        } while (integral);

        if (buf.size() > 1) {
            std::for_each(buf.begin(), std::prev(buf.end()), [](auto& elem) {
                // set msb
                elem |= 0x80;
            });
        }
    }

    template <typename Iterator,
              typename = std::enable_if_t<
                  sizeof(typename Iterator::value_type) == 1 &&
                  std::is_integral_v<typename Iterator::value_type>>>
    Varint(Iterator begin, Iterator end)
        : buf(begin, end) {}

    auto size() const { return buf.size(); }

    auto begin() const { return buf.cbegin(); }

    auto end() const { return buf.cend(); }

    operator std::uint64_t() const {
        std::uint64_t ret{};
        for (auto i = 0; i < size(); ++i)
            ret |= (buf[i] & 0x7f) << (7 * i);

        return ret;
    }
};

/**
 * @brief Extract Varint from sequence.
 *
 * @param begin Iterator to beginning of Varint
 * @param end Iterator to end of container
 * @return Deserialized Varint and past-the-end Iterator
 *
 * @throw std::runtime_error If Varint size is too big, or error in parsing */
template <typename Iterator>
std::tuple<Varint, Iterator> make_varint(Iterator begin, Iterator end) {
    if (std::all_of(begin, end, [](auto& elem) { return elem & 0x80; }))
        throw std::runtime_error("cannot convert to varint");

    std::uint64_t tmp{};
    auto it = begin;

    for (auto offset = 0; it != end; ++it, offset += 7) {
        if (std::distance(begin, it) > 9)
            throw std::runtime_error("varint too large");

        tmp |= (*it & 0x7f) << offset;

        if (!(*it & 0x80))
            break;
    }

    return {{tmp}, it};
}
