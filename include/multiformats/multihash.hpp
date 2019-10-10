// Multihash -- self-describing hashes
//
// Author: Matthew Knight
// File Name: multihash.hpp
// Date: 2019-09-12

#pragma once

#include <vector>

#include <cstdint>

namespace Multiformat::Multihash {
    std::vector<std::uint8_t> encode(std::vector<std::uint8_t> const& buf);
    std::vector<std::uint8_t> decode(std::vector<std::uint8_t> const& buf);
}
