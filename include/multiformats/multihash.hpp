// Multihash -- self-describing hashes
//
// Author: Matthew Knight
// File Name: multihash.hpp
// Date: 2019-09-12

#pragma once

#include "multiformats/varint.hpp"

#include <vector>

#include <cstdint>

namespace Multiformats::Multihash {
    std::vector<std::uint8_t> encode(std::vector<std::uint8_t> const& buf,
                                     std::string const& protocol);
    std::vector<std::uint8_t> encode(std::vector<std::uint8_t> const& buf,
                                     Varint const& protocol);
    std::vector<std::uint8_t> decode(std::vector<std::uint8_t> const& buf);
} // namespace Multiformats::Multihash
